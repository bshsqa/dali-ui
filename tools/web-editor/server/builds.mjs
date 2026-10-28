import { spawn } from 'node:child_process';
import { randomUUID } from 'node:crypto';
import { promises as fs } from 'node:fs';
import path from 'node:path';
import { HttpError } from './workspace.mjs';

export class Builds {
  constructor(workspace, buildRoot, broadcast) {
    this.workspace = workspace;
    this.buildRoot = buildRoot;
    this.broadcast = broadcast;
    this.current = null;
    this.child = null;
    this.previewPaths = new Map();
  }

  snapshot() {
    return this.current;
  }

  async start(target) {
    if (this.child) throw new HttpError(409, 'A build is already running. Stop it or wait for it to finish.');
    // Reserve before the async sample lookup, so concurrent requests cannot spawn two builds.
    this.child = { reserved: true };
    try {
      const sample = (await this.workspace.samples()).find(sample => sample.target === target);
      if (!sample) throw new HttpError(400, 'Select a sample target defined by DALI_ADD_SAMPLE.');
      const id = randomUUID();
      this.current = { id, target, directory: sample.directory, status: 'building',
        startedAt: Date.now(), finishedAt: null, exitCode: null, output: '', diagnostics: [] };
      const args = [path.join(this.workspace.root, 'dali-adaptor/build/web/build.sh'), 'all',
        '--samples', sample.directory, '--targets', sample.target];
      const command = 'bash ' + args.map(arg => JSON.stringify(arg)).join(' ');
      this.current.command = command;
      this.broadcast({ type: 'build.started', build: this.snapshot() });
      this.append(`\x1b[36m$ ${command}\x1b[0m\r\n`);
      const child = spawn('bash', args, { cwd: this.workspace.root, detached: true,
        env: { ...process.env, DALI_WEB_BUILD: this.buildRoot, DALI_WEB_VERBOSE: '1',
          CMAKE_BUILD_PARALLEL_LEVEL: process.env.JOBS || '8' }, stdio: ['ignore', 'pipe', 'pipe'] });
      this.child = child;
      child.stdout.on('data', data => this.append(data.toString()));
      child.stderr.on('data', data => this.append(data.toString()));
      child.on('error', error => this.append(`\r\n${error.message}\r\n`));
      child.on('close', async (code, signal) => {
        let publicationFailed = false;
        if (code === 0 && this.current.status !== 'cancelling') {
          try {
            this.current.previewPath = await this.publish(sample.target, id);
            if (this.current.status !== 'cancelling') this.previewPaths.set(sample.target, this.current.previewPath);
          } catch (error) {
            publicationFailed = true;
            this.append(`\r\nCould not publish the preview: ${error.message}\r\n`);
          }
        }
        const cancelled = this.current.status === 'cancelling';
        Object.assign(this.current, { status: cancelled ? 'cancelled' : code === 0 && !publicationFailed ? 'succeeded' : 'failed',
          finishedAt: Date.now(), exitCode: code, signal });
        this.current.diagnostics = this.diagnostics(this.current.output);
        this.child = null;
        this.broadcast({ type: 'build.finished', build: this.snapshot() });
      });
      return { ...this.current, output: '' };
    } catch (error) {
      this.child = null;
      throw error;
    }
  }

  append(text) {
    if (!this.current) return;
    this.current.output = (this.current.output + text).slice(-1024 * 1024);
    this.broadcast({ type: 'build.output', id: this.current.id, data: text });
  }

  async publish(target, id) {
    const source = path.join(this.buildRoot, 'samples/dist');
    const snapshots = path.join(this.buildRoot, 'editor-runs');
    const temporary = path.join(snapshots, `${id}.publishing`);
    const destination = path.join(snapshots, id);
    await Promise.all(['html', 'js', 'wasm'].map(extension => fs.access(path.join(source, `${target}.${extension}`))));
    await fs.mkdir(temporary, { recursive: true });
    try {
      const files = (await fs.readdir(source)).filter(name => name.startsWith(target + '.'));
      await Promise.all(files.map(name => fs.copyFile(path.join(source, name), path.join(temporary, name))));
      await fs.rename(temporary, destination);
      return `runs/${id}/${target}.html`;
    } catch (error) {
      await fs.rm(temporary, { recursive: true, force: true });
      throw error;
    }
  }

  diagnostics(output) {
    const results = [];
    const clean = output.replace(/\x1b\[[0-9;]*m/g, '');
    for (const match of clean.matchAll(/^(.+?):(\d+):(\d+):\s*(fatal error|error|warning):\s*(.+)$/gm)) {
      const relative = path.relative(this.workspace.root, path.resolve(this.workspace.root, match[1]));
      if (relative.startsWith('..') || path.isAbsolute(relative)) continue;
      results.push({ path: relative, line: Number(match[2]), column: Number(match[3]),
        severity: match[4] === 'warning' ? 'warning' : 'error', message: match[5] });
    }
    return results.slice(0, 100);
  }

  cancel() {
    if (!this.child) return false;
    if (this.child.reserved) throw new HttpError(409, 'The build is starting. Try stopping it again.');
    const child = this.child;
    this.current.status = 'cancelling';
    this.broadcast({ type: 'build.state', build: this.snapshot() });
    try { process.kill(-child.pid, 'SIGTERM'); } catch (error) { if (error.code !== 'ESRCH') throw error; }
    const timer = setTimeout(() => {
      if (this.child === child) {
        try { process.kill(-child.pid, 'SIGKILL'); } catch {}
      }
    }, 3000);
    timer.unref();
    return true;
  }
}
