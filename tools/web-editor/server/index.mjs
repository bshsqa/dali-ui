import http from 'node:http';
import { createReadStream, promises as fs } from 'node:fs';
import { randomBytes } from 'node:crypto';
import { fileURLToPath } from 'node:url';
import path from 'node:path';
import { parseArgs } from 'node:util';
import pty from 'node-pty';
import { WebSocketServer, WebSocket } from 'ws';
import { Workspace, HttpError } from './workspace.mjs';
import { Builds } from './builds.mjs';

const toolRoot = fileURLToPath(new URL('..', import.meta.url));
const defaultWorkspace = path.resolve(toolRoot, '../../..');
const mime = { '.html': 'text/html; charset=utf-8', '.js': 'text/javascript; charset=utf-8',
  '.css': 'text/css; charset=utf-8', '.json': 'application/json', '.wasm': 'application/wasm',
  '.svg': 'image/svg+xml', '.png': 'image/png', '.jpg': 'image/jpeg', '.woff2': 'font/woff2' };

function json(response, code, value) {
  response.writeHead(code, { 'Content-Type': 'application/json; charset=utf-8' });
  response.end(JSON.stringify(value));
}

async function body(request) {
  let size = 0;
  const parts = [];
  for await (const part of request) {
    size += part.length;
    if (size > 3 * 1024 * 1024) throw new HttpError(413, 'Request too large.');
    parts.push(part);
  }
  try { return JSON.parse(Buffer.concat(parts).toString()); }
  catch { throw new HttpError(400, 'Expected a JSON request.'); }
}

async function serveFile(request, response, base, relative) {
  const root = await fs.realpath(base);
  const target = await fs.realpath(path.resolve(root, relative));
  if (!target.startsWith(root + path.sep)) throw new HttpError(403, 'Invalid asset path.');
  const stat = await fs.stat(target);
  if (!stat.isFile()) throw new HttpError(404, 'File not found.');
  response.writeHead(200, { 'Content-Type': mime[path.extname(target)] || 'application/octet-stream',
    'Content-Length': stat.size });
  if (request.method === 'HEAD') response.end();
  else createReadStream(target).on('error', () => response.destroy()).pipe(response);
}

function fail(response, error) {
  const status = error.status || (error.code === 'ENOENT' ? 404 : error.code === 'EEXIST' ? 409 : 500);
  if (!response.headersSent) json(response, status, { error: status === 500 ? 'The backend could not complete the request.' : error.message });
  else response.destroy();
  if (status === 500) console.error(error);
}

export async function startEditor(options = {}) {
  const root = await fs.realpath(options.workspace || process.env.DALI_EDITOR_WORKSPACE || defaultWorkspace);
  const workspace = new Workspace(root);
  await Promise.all(['dali-core', 'dali-adaptor', 'dali-ui'].map(name => fs.access(path.join(root, name))));
  const buildRoot = path.resolve(options.buildRoot || process.env.DALI_WEB_BUILD || '/tmp/dali-web-build');
  const previewDirectory = path.join(buildRoot, 'samples/dist');
  const host = options.host || '127.0.0.1';
  const token = randomBytes(32).toString('hex');
  const sockets = new Set();
  const origins = new Set();
  let previewPort;
  const broadcast = value => {
    const data = JSON.stringify(value);
    for (const socket of sockets) {
      if (socket.readyState === WebSocket.OPEN && socket.bufferedAmount < 16 * 1024 * 1024) socket.send(data);
    }
  };
  const builds = new Builds(workspace, buildRoot, broadcast);
  const previewOrigin = request => {
    if (options.previewOrigin) return options.previewOrigin;
    const hostname = new URL(`http://${request.headers.host}`).hostname;
    return `http://${hostname}:${previewPort}`;
  };
  const headers = (request, response, editor = false) => {
    response.setHeader('Cross-Origin-Opener-Policy', 'same-origin');
    response.setHeader('Cross-Origin-Embedder-Policy', 'require-corp');
    response.setHeader('Cache-Control', 'no-store');
    response.setHeader('X-Content-Type-Options', 'nosniff');
    response.setHeader('Referrer-Policy', 'strict-origin');
    if (editor) response.setHeader('Permissions-Policy', `cross-origin-isolated=(self "${previewOrigin(request)}")`);
  };
  const authenticated = request => (request.headers.cookie || '').split(/;\s*/).includes(`dali_editor_session=${token}`);
  const validOrigin = request => !request.headers.origin || origins.has(request.headers.origin);

  // Preview code runs on a separate origin. This listener exposes only build
  // artifacts; it has no file-editing, shell, or build endpoints.
  const previewServer = http.createServer(async (request, response) => {
    headers(request, response);
    response.setHeader('Cross-Origin-Resource-Policy', 'cross-origin');
    try {
      if (!['GET', 'HEAD'].includes(request.method)) throw new HttpError(405, 'Read-only preview server.');
      const url = new URL(request.url, 'http://preview.local');
      const relative = decodeURIComponent(url.pathname).replace(/^\/+/, '') || 'index.html';
      if (relative.startsWith('runs/')) await serveFile(request, response, path.join(buildRoot, 'editor-runs'), relative.slice(5));
      else await serveFile(request, response, previewDirectory, relative);
    } catch (error) { fail(response, error); }
  });
  await new Promise((resolve, reject) => {
    previewServer.once('error', reject);
    previewServer.listen(options.previewPort ?? 8788, host, resolve);
  });
  previewPort = previewServer.address().port;

  const server = http.createServer(async (request, response) => {
    try {
      headers(request, response, true);
      const hostOrigin = `http://${request.headers.host}`;
      const allowedHost = origins.has(hostOrigin) || (options.origin && new URL(options.origin).host === request.headers.host);
      if (!allowedHost) throw new HttpError(403, 'Unrecognized editor host.');
      const url = new URL(request.url, hostOrigin);
      if (url.pathname.startsWith('/api/')) {
        if (!authenticated(request) || !validOrigin(request)) throw new HttpError(403, 'Open the editor page to start a session.');
        if (url.pathname !== '/api/config' && request.headers['x-dali-editor-token'] !== token) {
          throw new HttpError(403, 'Invalid editor session.');
        }
        if (url.pathname === '/api/config' && request.method === 'GET') {
          return json(response, 200, { name: path.basename(root), root, token,
            samples: await workspace.samples(), previewOrigin: previewOrigin(request),
            readyTargets: (await fs.readdir(previewDirectory).catch(() => []))
              .filter(name => name.endsWith('.html')).map(name => name.slice(0, -5)),
            previewPaths: Object.fromEntries(builds.previewPaths),
            defaultTarget: 'text-gradient.example', build: builds.snapshot() });
        }
        if (url.pathname === '/api/tree' && request.method === 'GET') {
          return json(response, 200, { entries: await workspace.tree(url.searchParams.get('path') || '') });
        }
        if (url.pathname === '/api/file' && request.method === 'GET') {
          return json(response, 200, await workspace.read(url.searchParams.get('path')));
        }
        if (url.pathname === '/api/file' && request.method === 'PUT') {
          const data = await body(request);
          return json(response, 200, await workspace.save(data.path, data.content, data.revision));
        }
        if (url.pathname === '/api/files' && request.method === 'POST') {
          const data = await body(request);
          return json(response, 201, await workspace.create(data.path, data.type));
        }
        if (url.pathname === '/api/builds' && request.method === 'POST') {
          const data = await body(request);
          return json(response, 202, await builds.start(data.target));
        }
        if (url.pathname === '/api/builds' && request.method === 'GET') {
          return json(response, 200, { build: builds.snapshot() });
        }
        if (url.pathname === '/api/builds/current' && request.method === 'DELETE') {
          return json(response, 200, { cancelled: builds.cancel() });
        }
        throw new HttpError(404, 'Unknown API endpoint.');
      }
      if (!['GET', 'HEAD'].includes(request.method)) throw new HttpError(405, 'Method not allowed.');
      const relative = decodeURIComponent(url.pathname).replace(/^\/+/, '') || 'index.html';
      if (relative === 'index.html') {
        response.setHeader('Set-Cookie', `dali_editor_session=${token}; Path=/; HttpOnly; SameSite=Strict`);
      }
      await serveFile(request, response, path.join(toolRoot, 'dist'), relative);
    } catch (error) { fail(response, error); }
  });

  const wss = new WebSocketServer({ noServer: true, maxPayload: 128 * 1024,
    handleProtocols: protocols => protocols.has('dali-editor') ? 'dali-editor' : false });
  server.on('upgrade', (request, socket, head) => {
    const protocols = (request.headers['sec-websocket-protocol'] || '').split(/,\s*/);
    if (request.url !== '/ws' || !authenticated(request) || !origins.has(request.headers.origin)
      || !protocols.includes(token)) {
      socket.end('HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n');
      return;
    }
    wss.handleUpgrade(request, socket, head, client => wss.emit('connection', client));
  });
  wss.on('connection', socket => {
    sockets.add(socket);
    let terminal;
    const send = value => {
      if (socket.readyState === WebSocket.OPEN) socket.send(JSON.stringify(value));
    };
    const killTerminal = () => {
      const previous = terminal;
      terminal = null;
      if (previous) {
        try { process.kill(-previous.pid, 'SIGHUP'); } catch {}
        try { previous.kill(); } catch {}
      }
    };
    const openTerminal = () => {
      killTerminal();
      try {
        const child = pty.spawn('/bin/bash', ['--noprofile', '--norc'], { name: 'xterm-256color',
          cols: 80, rows: 24, cwd: root,
          env: { ...process.env, TERM: 'xterm-256color', COLORTERM: 'truecolor',
            PS1: '\\[\\e[38;5;75m\\]dali\\[\\e[0m\\]:\\w \\$ ' } });
        terminal = child;
        child.onData(data => { if (terminal === child) send({ type: 'terminal.output', data }); });
        child.onExit(({ exitCode }) => {
          if (terminal === child) { terminal = null; send({ type: 'terminal.exit', exitCode }); }
        });
        send({ type: 'terminal.ready', pid: child.pid });
      } catch (error) { send({ type: 'terminal.error', message: error.message }); }
    };
    send({ type: 'build.state', build: builds.snapshot() });
    openTerminal();
    socket.on('message', raw => {
      try {
        const value = JSON.parse(raw.toString());
        if (value.type === 'terminal.input' && typeof value.data === 'string' && value.data.length <= 65536) terminal?.write(value.data);
        else if (value.type === 'terminal.resize' && Number.isInteger(value.cols) && Number.isInteger(value.rows)
          && value.cols >= 2 && value.cols <= 500 && value.rows >= 2 && value.rows <= 300) terminal?.resize(value.cols, value.rows);
        else if (value.type === 'terminal.restart') openTerminal();
      } catch { send({ type: 'terminal.error', message: 'Invalid terminal message.' }); }
    });
    socket.on('close', () => { sockets.delete(socket); killTerminal(); });
    socket.on('error', () => socket.close());
  });

  try {
    await new Promise((resolve, reject) => {
      server.once('error', reject);
      server.listen(options.port ?? 8787, host, resolve);
    });
  } catch (error) { previewServer.close(); throw error; }
  const port = server.address().port;
  origins.add(`http://127.0.0.1:${port}`);
  origins.add(`http://localhost:${port}`);
  if (options.origin) origins.add(options.origin);
  return { server, previewServer, builds, workspace, port, previewPort,
    async close() {
      if (builds.child && !builds.child.reserved) builds.cancel();
      for (const socket of sockets) socket.terminate();
      wss.close();
      server.closeAllConnections();
      previewServer.closeAllConnections();
      await Promise.all([new Promise(resolve => server.close(resolve)),
        new Promise(resolve => previewServer.close(resolve))]);
    } };
}

if (process.argv[1] && path.resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  const { values } = parseArgs({ options: { workspace: { type: 'string' }, host: { type: 'string' },
    port: { type: 'string', default: '8787' }, 'preview-port': { type: 'string', default: '8788' },
    origin: { type: 'string' }, 'preview-origin': { type: 'string' }, help: { type: 'boolean' } } });
  if (values.help) {
    console.log('DALi Web Editor: npm start -- [--workspace /path/to/DALi] [--port 8787] [--preview-port 8788]');
    process.exit(0);
  }
  const app = await startEditor({ workspace: values.workspace, host: values.host,
    port: Number(values.port), previewPort: Number(values['preview-port']),
    origin: values.origin || process.env.DALI_EDITOR_ORIGIN,
    previewOrigin: values['preview-origin'] || process.env.DALI_EDITOR_PREVIEW_ORIGIN });
  console.log(`DALi Web Editor: http://127.0.0.1:${app.port}/`);
  console.log(`Workspace: ${app.workspace.root}`);
  console.log(`Preview artifacts: http://127.0.0.1:${app.previewPort}/`);
  for (const signal of ['SIGINT', 'SIGTERM']) process.once(signal, async () => { await app.close(); });
}
