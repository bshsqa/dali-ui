import test from 'node:test';
import assert from 'node:assert/strict';
import { promises as fs } from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { setTimeout as delay } from 'node:timers/promises';
import { WebSocket } from 'ws';
import { Workspace } from '../server/workspace.mjs';
import { Builds } from '../server/builds.mjs';
import { startEditor } from '../server/index.mjs';

async function fixture(t) {
  const root = await fs.mkdtemp(path.join(os.tmpdir(), 'dali-editor-test-'));
  t.after(() => fs.rm(root, { recursive: true, force: true }));
  for (const directory of ['dali-core', 'dali-adaptor/build/web', 'dali-ui/samples/hello-world', 'dali-ui/samples/text']) {
    await fs.mkdir(path.join(root, directory), { recursive: true });
  }
  await fs.writeFile(path.join(root, 'dali-ui/samples/hello-world/CMakeLists.txt'),
    'DALI_ADD_SAMPLE(hello-world.example hello-world-example.cpp)\n');
  await fs.writeFile(path.join(root, 'dali-ui/samples/hello-world/hello-world-example.cpp'), 'int main() {}\n');
  await fs.writeFile(path.join(root, 'dali-ui/samples/text/CMakeLists.txt'),
    'DALI_ADD_SAMPLE(text-gradient.example text-gradient-example.cpp)\n');
  return { root, workspace: new Workspace(root) };
}

async function waitFor(predicate, timeout = 5000) {
  const end = Date.now() + timeout;
  while (Date.now() < end) { if (await predicate()) return; await delay(20); }
  throw new Error('Timed out waiting for the expected backend state');
}

test('workspace saves real files, rejects stale revisions, traversal, and escaping symlinks', async t => {
  const { root, workspace } = await fixture(t);
  const file = 'dali-ui/samples/hello-world/hello-world-example.cpp';
  const initial = await workspace.read(file);
  const result = await workspace.save(file, 'int main() { return 7; }\n', initial.revision);
  assert.equal(await fs.readFile(path.join(root, file), 'utf8'), 'int main() { return 7; }\n');
  await assert.rejects(workspace.save(file, 'stale', initial.revision), error => error.status === 409);
  assert.notEqual(result.revision, initial.revision);
  await assert.rejects(workspace.read('../outside'), error => error.status === 403);
  await fs.symlink('/etc', path.join(root, 'escape'));
  await assert.rejects(workspace.read('escape/passwd'), error => error.status === 403);
  await assert.rejects(workspace.create('escape/should-not-exist', 'file'), error => error.status === 403);
  await workspace.create('dali-ui/samples/hello-world/new.h', 'file');
  assert.equal((await workspace.read('dali-ui/samples/hello-world/new.h')).content, '');
});

test('builds stream child output and diagnostics, serialize jobs, and cancel process groups', async t => {
  const { root, workspace } = await fixture(t);
  const helper = path.join(root, 'dali-adaptor/build/web/build.sh');
  const events = [];
  const builds = new Builds(workspace, path.join(root, 'out'), value => events.push(value));
  await fs.writeFile(helper, 'printf "%s:3:2: error: expected expression\\n" "$PWD/dali-ui/samples/hello-world/hello-world-example.cpp"\nexit 1\n');
  await builds.start('hello-world.example');
  await waitFor(() => builds.snapshot().status === 'failed');
  assert(events.some(event => event.type === 'build.output' && event.data.includes('expected expression')));
  assert.equal(builds.snapshot().diagnostics[0].line, 3);
  assert.equal(builds.snapshot().diagnostics[0].path, 'dali-ui/samples/hello-world/hello-world-example.cpp');
  await fs.writeFile(helper, 'mkdir -p "$DALI_WEB_BUILD/samples/dist"\nfor extension in html js wasm data; do\n printf original > "$DALI_WEB_BUILD/samples/dist/hello-world.example.$extension"\ndone\n');
  await builds.start('hello-world.example');
  await waitFor(() => builds.snapshot().status === 'succeeded');
  const published = builds.snapshot().previewPath;
  assert.match(published, /^runs\/[\w-]+\/hello-world.example.html$/);
  const snapshot = path.join(root, 'out/editor-runs', published.slice(5));
  await fs.writeFile(path.join(root, 'out/samples/dist/hello-world.example.html'), 'overwritten');
  assert.equal(await fs.readFile(snapshot, 'utf8'), 'original', 'New builds must not overwrite a published preview');
  await fs.writeFile(helper, 'echo waiting\nsleep 30\n');
  await builds.start('hello-world.example');
  await assert.rejects(builds.start('hello-world.example'), error => error.status === 409);
  const childPid = builds.child.pid;
  builds.cancel();
  await waitFor(() => builds.snapshot().status === 'cancelled');
  assert.throws(() => process.kill(childPid, 0), error => error.code === 'ESRCH');
  await assert.rejects(builds.start('bad; touch injected'), error => error.status === 400);
  assert.equal(builds.child, null);
});

test('HTTP and WebSocket connect to real Bash; session checks and preview separation hold', async t => {
  const { root } = await fixture(t);
  const buildRoot = path.join(root, 'out');
  await fs.mkdir(path.join(buildRoot, 'samples/dist'), { recursive: true });
  await fs.writeFile(path.join(buildRoot, 'samples/dist/index.html'), '<p>preview</p>');
  const app = await startEditor({ workspace: root, buildRoot, port: 0, previewPort: 0 });
  t.after(() => app.close());
  const origin = `http://127.0.0.1:${app.port}`;
  const home = await fetch(origin);
  assert.equal(home.status, 200);
  const cookie = home.headers.get('set-cookie').split(';')[0];
  const config = await (await fetch(origin + '/api/config', { headers: { cookie } })).json();
  assert(config.samples.some(sample => sample.target === 'hello-world.example'));
  assert.equal(config.defaultTarget, 'text-gradient.example');
  assert.deepEqual(config.samples.find(sample => sample.target === config.defaultTarget).sceneSize, [920, 880]);
  assert.deepEqual(config.samples.find(sample => sample.target === 'hello-world.example').sceneSize, [800, 600]);
  const headers = { cookie, origin, 'x-dali-editor-token': config.token };
  assert.equal((await fetch(origin + '/api/file?path=dali-ui/samples/hello-world/hello-world-example.cpp', { headers })).status, 200);
  assert.equal((await fetch(origin + '/api/tree', { headers: { cookie } })).status, 403);
  assert.equal((await fetch(origin + '/api/tree', { headers: { ...headers, origin: 'http://attacker.invalid' } })).status, 403);
  assert.equal((await fetch(config.previewOrigin + '/api/config')).status, 404);
  const preview = await fetch(config.previewOrigin + '/index.html');
  assert.equal(preview.headers.get('cross-origin-resource-policy'), 'cross-origin');
  const socket = new WebSocket(origin.replace('http:', 'ws:') + '/ws', ['dali-editor', config.token], { headers: { cookie, origin } });
  t.after(() => socket.terminate());
  const messages = [];
  socket.on('message', raw => messages.push(JSON.parse(raw)));
  await waitFor(() => messages.some(value => value.type === 'terminal.ready'));
  const pid = messages.find(value => value.type === 'terminal.ready').pid;
  socket.send(JSON.stringify({ type: 'terminal.input', data: 'printf "%s\\n" "$((19+23))"\r' }));
  await waitFor(() => /(?:^|[\r\n])42\r?\n/.test(messages.filter(value => value.type === 'terminal.output').map(value => value.data).join('')));
  socket.close();
  await waitFor(() => { try { process.kill(pid, 0); return false; } catch { return true; } });
});
