import { createHash, randomUUID } from 'node:crypto';
import { promises as fs } from 'node:fs';
import path from 'node:path';

const excluded = new Set(['.git', 'node_modules', 'CMakeFiles', '.cache', '.local', 'dist', '__pycache__']);
const limit = 2 * 1024 * 1024;
export const verifiedSamples = new Set(['hello-world.example', 'animation.example', 'text.example',
  'image-view.example', 'stacklayout.example', 'flexlayout.example', 'gridlayout.example',
  'scrollview.example', 'key-input.example', 'absolutelayout.example', 'text-gradient.example']);

const sampleSceneSizes = { 'text-gradient.example': [920, 880] };

export class HttpError extends Error {
  constructor(status, message) { super(message); this.status = status; }
}

const revision = data => createHash('sha256').update(data).digest('hex');

export class Workspace {
  constructor(root) { this.root = root; this.saving = new Set(); }

  async resolve(relative = '', creating = false) {
    if (typeof relative !== 'string' || relative.includes('\0') || path.isAbsolute(relative)
      || relative.split(/[\\/]/).some(part => part === '..' || excluded.has(part))) {
      throw new HttpError(403, 'The path must stay inside the workspace.');
    }
    const target = path.resolve(this.root, relative);
    const real = creating ? path.join(await fs.realpath(path.dirname(target)), path.basename(target))
      : await fs.realpath(target);
    if (real !== this.root && !real.startsWith(this.root + path.sep)) {
      throw new HttpError(403, 'The path points outside the workspace.');
    }
    return real;
  }

  async tree(relative = '') {
    const directory = await this.resolve(relative);
    const entries = await fs.readdir(directory, { withFileTypes: true });
    return entries.filter(entry => !excluded.has(entry.name) && !entry.name.startsWith('.dali-editor-')
      && !entry.isSymbolicLink() && (entry.isDirectory() || entry.isFile()))
      .map(entry => ({ name: entry.name, path: path.relative(this.root, path.join(directory, entry.name)),
        type: entry.isDirectory() ? 'directory' : 'file' }))
      .sort((a, b) => Number(b.type === 'directory') - Number(a.type === 'directory') || a.name.localeCompare(b.name));
  }

  async read(relative) {
    const target = await this.resolve(relative);
    const stat = await fs.stat(target);
    if (!stat.isFile()) throw new HttpError(400, 'Select a text file.');
    if (stat.size > limit) throw new HttpError(413, 'The editor supports text files up to 2 MiB.');
    const data = await fs.readFile(target);
    if (data.subarray(0, 8192).includes(0)) throw new HttpError(415, 'Binary files cannot be edited.');
    return { path: path.relative(this.root, target), content: data.toString('utf8'), revision: revision(data) };
  }

  async save(relative, content, expectedRevision) {
    if (typeof content !== 'string' || Buffer.byteLength(content) > limit) {
      throw new HttpError(413, 'The editor supports text files up to 2 MiB.');
    }
    const target = await this.resolve(relative);
    if (this.saving.has(target)) throw new HttpError(409, 'This file is already being saved. Try again.');
    this.saving.add(target);
    let temporary;
    try {
      const previous = await this.read(relative);
      if (!expectedRevision || previous.revision !== expectedRevision) {
        throw new HttpError(409, 'The file changed on disk. Reopen it before saving to avoid overwriting those changes.');
      }
      const stat = await fs.stat(target);
      temporary = path.join(path.dirname(target), `.dali-editor-${randomUUID()}.tmp`);
      await fs.writeFile(temporary, content, { flag: 'wx', mode: stat.mode });
      if (revision(await fs.readFile(target)) !== expectedRevision) {
        throw new HttpError(409, 'The file changed on disk while saving. Reopen it and retry.');
      }
      await fs.rename(temporary, target);
      return { path: relative, revision: revision(Buffer.from(content)) };
    } finally {
      if (temporary) await fs.unlink(temporary).catch(() => {});
      this.saving.delete(target);
    }
  }

  async create(relative, type) {
    if (!relative) throw new HttpError(400, 'Enter a workspace-relative path.');
    const target = await this.resolve(relative, true);
    if (type === 'directory') await fs.mkdir(target);
    else if (type === 'file') await fs.writeFile(target, '', { flag: 'wx' });
    else throw new HttpError(400, 'Unknown file type.');
    return { path: path.relative(this.root, target), type };
  }

  async samples() {
    const base = path.join(this.root, 'dali-ui/samples');
    const results = [];
    for (const entry of await fs.readdir(base, { withFileTypes: true })) {
      if (!entry.isDirectory() || entry.name.startsWith('.')) continue;
      const cmake = await fs.readFile(path.join(base, entry.name, 'CMakeLists.txt'), 'utf8').catch(() => '');
      for (const match of cmake.matchAll(/DALI_ADD_SAMPLE\(\s*([\w.-]+)\s+([^)]*)\)/gi)) {
        const source = match[2].match(/[\w./-]+\.(?:cpp|cc|cxx)\b/)?.[0];
        results.push({ target: match[1], directory: entry.name,
          source: source ? path.join('dali-ui/samples', entry.name, source) : null,
          verified: verifiedSamples.has(match[1]) });
      }
      // Image samples use paired CMake lists expanded by a FOREACH loop.
      // Read the declaration as metadata; never evaluate CMake or a shell here.
      for (const list of cmake.matchAll(/SET\(\s*(\w+)_SAMPLES\s+([^)]*)\)/gi)) {
        const sources = cmake.match(new RegExp(`SET\\(\\s*${list[1]}_SOURCES\\s+([^)]*)\\)`, 'i'));
        if (!sources) continue;
        const targets = list[2].match(/[\w.-]+\.example\b/g) || [];
        const files = sources[1].match(/[\w./-]+\.(?:cpp|cc|cxx)\b/g) || [];
        if (targets.length !== files.length) continue;
        targets.forEach((target, index) => {
          if (!results.some(sample => sample.target === target)) results.push({ target, directory: entry.name,
            source: path.join('dali-ui/samples', entry.name, files[index]), verified: verifiedSamples.has(target) });
        });
      }
    }
    return results.map(sample => ({ ...sample, sceneSize: sampleSceneSizes[sample.target] || [800, 600] }))
      .sort((a, b) => Number(b.verified) - Number(a.verified) || a.target.localeCompare(b.target));
  }
}
