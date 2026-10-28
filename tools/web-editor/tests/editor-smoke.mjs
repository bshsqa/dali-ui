// Real browser + real Emscripten builds. Creates and removes only its own sample.
import assert from 'node:assert/strict';
import { promises as fs } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { chromium } from 'playwright-core';
import { PNG } from 'pngjs';

const base = process.env.DALI_EDITOR_TEST_URL || 'http://127.0.0.1:8787/';
const output = process.env.DALI_EDITOR_TEST_OUTPUT || '/tmp/dali-editor-test-results';
const root = path.resolve(fileURLToPath(new URL('../../..', import.meta.url)));
const directory = `web-editor-smoke-${Date.now()}`;
const target = `${directory}.example`;
const relative = `dali-ui/samples/${directory}`;
const sourceFile = `${relative}/app.cpp`;
const results = [];
await fs.mkdir(output, { recursive: true });
const browser = await chromium.launch({ executablePath: process.env.DALI_WEB_TEST_BROWSER || '/usr/bin/google-chrome',
  headless: true, args: ['--no-sandbox', '--use-angle=swiftshader', '--enable-unsafe-swiftshader'] });
const context = await browser.newContext({ viewport: { width: 1600, height: 1000 }, deviceScaleFactor: 1,
  permissions: ['clipboard-read', 'clipboard-write'] });
const page = await context.newPage();
const fatal = [];
let shellOutput = '';
let config;
page.setDefaultTimeout(30000);
page.on('pageerror', error => fatal.push(error.message));
page.on('console', message => {
  fs.appendFile(path.join(output, 'browser.log'), `${message.type()}: ${message.text()}\n`).catch(() => {});
  if (/WebGL: INVALID_|Aborted\(|RuntimeError:|Assertion failed/.test(message.text())) fatal.push(message.text());
});
page.on('websocket', socket => socket.on('framereceived', ({ payload }) => {
  try { const message = JSON.parse(payload); if (message.type === 'terminal.output') shellOutput += message.data; } catch {}
}));
function pass(name) { results.push({ test: name, passed: true }); console.log(`PASS ${name}`); }
async function request(endpoint, options = {}) {
  const response = await page.request.fetch(new URL(`/api/${endpoint}`, base).href, {
    ...options, headers: { 'Content-Type': 'application/json', 'X-Dali-Editor-Token': config?.token || '', ...options.headers },
  });
  const value = await response.json();
  assert(response.ok(), `${endpoint}: ${JSON.stringify(value)}`);
  return value;
}
async function waitUntil(predicate, timeout = 30000) {
  const deadline = Date.now() + timeout;
  while (Date.now() < deadline) { if (await predicate()) return; await page.waitForTimeout(100); }
  throw new Error('Timed out waiting for editor state');
}
function colorBox(image, color) {
  const png = PNG.sync.read(image);
  let left = png.width, top = png.height, right = 0, bottom = 0, count = 0;
  for (let y = 0; y < png.height; ++y) for (let x = 0; x < png.width; ++x) {
    const i = (y * png.width + x) * 4;
    if (color.every((channel, k) => png.data[i + k] === channel)) {
      left = Math.min(left, x); top = Math.min(top, y); right = Math.max(right, x); bottom = Math.max(bottom, y); ++count;
    }
  }
  return { left, top, right, bottom, count };
}
const canvas = () => page.frameLocator('#preview-frame').locator('canvas');
async function waitColor(color) {
  let box;
  await waitUntil(async () => {
    try { box = colorBox(await canvas().screenshot(), color); return box.count > 150; } catch { return false; }
  });
  return box;
}
async function gradientPixels() {
  const png = PNG.sync.read(await canvas().screenshot());
  const scale = Math.min(png.width / 920, png.height / 880);
  const offsetX = (png.width - 920 * scale) / 2;
  const offsetY = (png.height - 880 * scale) / 2;
  let count = 0;
  for (let y = Math.ceil(offsetY + 200 * scale); y < Math.floor(offsetY + 760 * scale); ++y) {
    for (let x = Math.ceil(offsetX); x < Math.floor(offsetX + 920 * scale); ++x) {
      const i = (y * png.width + x) * 4;
      const rgb = png.data.subarray(i, i + 3);
      if (Math.max(...rgb) - Math.min(...rgb) > 60) ++count;
    }
  }
  return count;
}
async function drag(selector, dx, dy) {
  const bounds = await page.locator(selector).boundingBox();
  const x = bounds.x + bounds.width / 2, y = bounds.y + bounds.height / 2;
  await page.mouse.move(x, y); await page.mouse.down();
  await page.mouse.move(x + dx, y + dy, { steps: 12 }); await page.mouse.up();
}
async function replaceEditor(content) {
  await page.locator('#editor .monaco-editor').click({ position: { x: 150, y: 55 } });
  await page.evaluate(text => navigator.clipboard.writeText(text), content);
  await page.keyboard.press('Control+a'); await page.keyboard.press('Control+v');
}
async function clickBuild() {
  const responsePromise = page.waitForResponse(response => response.url().endsWith('/api/builds') && response.request().method() === 'POST');
  await page.getByRole('button', { name: 'Build & Run' }).click();
  const response = await responsePromise;
  assert.equal(response.status(), 202);
  return (await response.json()).id;
}
async function finishBuild(id) {
  let build;
  await waitUntil(async () => {
    build = (await request('builds')).build;
    return build?.id === id && !['building', 'cancelling'].includes(build.status);
  }, 300000);
  await fs.writeFile(path.join(output, `build-${id}.log`), build.output);
  return build;
}

try {
  await page.goto(base);
  await page.locator('.monaco-editor').waitFor();
  config = await request('config');
  assert.equal(config.samples.filter(sample => sample.verified).length, 11);
  assert.equal(config.defaultTarget, 'text-gradient.example');
  assert.equal(await page.getByLabel('Application target').inputValue(), config.defaultTarget);
  assert.equal(await page.evaluate(() => crossOriginIsolated), true);
  await waitColor([29, 78, 216]);
  assert.equal(await page.locator('#scene-dimensions').innerText(), 'SCENE 920 × 880');
  await waitUntil(() => canvas().evaluate(element => Math.abs(element.width - element.getBoundingClientRect().width) < 2
    && Math.abs(element.height - element.getBoundingClientRect().height) < 2));
  await page.locator('.editor-tab.active', { hasText: 'text-gradient-example.cpp' }).waitFor();
  const gradientBefore = await gradientPixels();
  assert(gradientBefore > 200, 'The default app must draw its colored gradient text');
  const clear = await waitColor([127, 29, 29]);
  await canvas().click({ position: { x: (clear.left + clear.right) / 2, y: (clear.top + clear.bottom) / 2 } });
  await waitUntil(async () => (await gradientPixels()) < gradientBefore * 0.1);
  await page.keyboard.press('c');
  await waitUntil(async () => (await gradientPixels()) > gradientBefore * 0.5);
  await page.screenshot({ path: path.join(output, 'editor-default.png') });
  pass('default text-gradient app renders, fits the host, and responds to mouse and keyboard');
  await page.getByLabel('Application target').selectOption('hello-world.example');
  await waitColor([0, 0, 255]);
  assert.equal(await canvas().evaluate(element => element.ownerDocument.defaultView.crossOriginIsolated), true);
  await page.screenshot({ path: path.join(output, 'editor-initial.png') });
  pass('workspace explorer, source editor and isolated WASM preview');

  await page.locator('#terminal-shell .xterm-helper-textarea').focus();
  await page.keyboard.type('printf "EDITOR_CLI:%s\\n" "$((6*7))"'); await page.keyboard.press('Enter');
  await waitUntil(() => /(?:^|[\r\n])EDITOR_CLI:42\r?\n/.test(shellOutput));
  pass('interactive terminal executes a real Bash command');

  const sizeBefore = await canvas().evaluate(element => [element.width, element.height]);
  await drag('#explorer-splitter', 45, 0);
  await drag('#code-splitter', 90, 0);
  await drag('#terminal-splitter', 0, -90);
  await waitUntil(() => canvas().evaluate(element => Math.abs(element.width - element.getBoundingClientRect().width) < 2
    && Math.abs(element.height - element.getBoundingClientRect().height) < 2));
  const sizeAfter = await canvas().evaluate(element => [element.width, element.height]);
  assert.notDeepEqual(sizeAfter, sizeBefore);
  const blue = await waitColor([0, 0, 255]);
  assert.equal(await page.locator('#scene-dimensions').innerText(), 'SCENE 800 × 600');
  await canvas().click({ position: { x: (blue.left + blue.right) / 2, y: (blue.top + blue.bottom) / 2 } });
  await waitColor([0, 255, 0]);
  await page.screenshot({ path: path.join(output, 'editor-resized.png') });
  pass('all three splitters resize; fixed scene keeps rendering and touch coordinates');

  const original = (await request('file?path=dali-ui/samples/hello-world/hello-world-example.cpp')).content;
  const modified = original.replace('UiColor(0x0000FF)', 'UiColor(0xFF00FF)');
  assert.notEqual(modified, original);
  await request('files', { method: 'POST', data: { path: relative, type: 'directory' } });
  for (const [file, content] of [
    [`${relative}/CMakeLists.txt`, `cmake_minimum_required(VERSION 3.20)\nproject(${target})\nDALI_ADD_SAMPLE(${target} app.cpp)\n`],
    [sourceFile, original],
  ]) {
    await request('files', { method: 'POST', data: { path: file, type: 'file' } });
    const created = await request(`file?path=${encodeURIComponent(file)}`);
    await request('file', { method: 'PUT', data: { path: file, content, revision: created.revision } });
  }
  await page.reload(); await page.locator('.monaco-editor').waitFor(); config = await request('config');
  await page.getByLabel('Application target').selectOption(target);
  await page.locator('.editor-tab.active', { hasText: 'app.cpp' }).waitFor();
  await replaceEditor(modified);
  await page.locator('.editor-tab.active .dirty-marker').waitFor();
  const saveResponse = page.waitForResponse(response => response.url().endsWith('/api/file') && response.request().method() === 'PUT');
  await page.getByRole('button', { name: 'Save all' }).click();
  assert.equal((await saveResponse).status(), 200);
  assert.equal((await request(`file?path=${encodeURIComponent(sourceFile)}`)).content, modified);
  pass('editing and saving a local C++ file through Monaco');

  const successfulBuild = await finishBuild(await clickBuild());
  assert.equal(successfulBuild.status, 'succeeded', successfulBuild.output.slice(-4000));
  await waitColor([255, 0, 255]);
  const previousPreview = await page.locator('#preview-frame').getAttribute('src');
  assert(previousPreview.includes(target));
  await page.screenshot({ path: path.join(output, 'editor-built.png') });
  pass('Build & Run performs a real Emscripten build and launches the edited result');

  await replaceEditor(modified + '\n#error EDITOR_EXPECTED_BUILD_ERROR\n');
  const failedBuild = await finishBuild(await clickBuild());
  assert.equal(failedBuild.status, 'failed');
  assert(failedBuild.diagnostics.some(value => value.message.includes('EDITOR_EXPECTED_BUILD_ERROR')));
  assert.equal(await page.locator('#preview-frame').getAttribute('src'), previousPreview);
  await waitColor([255, 0, 255]);
  await page.locator('.problem', { hasText: 'EDITOR_EXPECTED_BUILD_ERROR' }).first().click();
  assert.match(await page.locator('#cursor-position').innerText(), /Ln \d+, Col/);
  await page.screenshot({ path: path.join(output, 'editor-error.png') });
  pass('compile errors link to source while the previous preview stays running');

  await replaceEditor(modified);
  const cancelled = await clickBuild(); await page.getByRole('button', { name: 'Stop', exact: true }).click();
  assert.equal((await finishBuild(cancelled)).status, 'cancelled');
  await page.locator('#preview-frame').waitFor({ state: 'hidden' });
  pass('Stop cancels the backend build and unloads the preview');

  await page.getByRole('button', { name: 'New file', exact: true }).click();
  await page.getByLabel('Path relative to workspace').fill(`${relative}/notes.txt`);
  await page.getByRole('button', { name: 'Create', exact: true }).click();
  await page.locator('.editor-tab.active', { hasText: 'notes.txt' }).waitFor();
  await replaceEditor('DALi editor file creation test\n');
  await page.keyboard.press('Control+s');
  await waitUntil(async () => (await request(`file?path=${encodeURIComponent(relative + '/notes.txt')}`)).content === 'DALi editor file creation test\n');
  pass('New file and Ctrl+S persist to the local workspace');
  assert.deepEqual(fatal, [], 'Browser, WASM or WebGL errors');
} catch (error) {
  await page.screenshot({ path: path.join(output, 'editor-failure.png') }).catch(() => {});
  if (config) {
    const file = await request(`file?path=${encodeURIComponent(sourceFile)}`).catch(() => null);
    if (file) await fs.writeFile(path.join(output, 'failed-source.cpp'), file.content);
  }
  throw error;
} finally {
  if (config) {
    const running = await request('builds').catch(() => ({}));
    if (['building', 'cancelling'].includes(running.build?.status)) {
      await request('builds/current', { method: 'DELETE' }).catch(() => {});
      await finishBuild(running.build.id).catch(() => {});
    }
  }
  await fs.writeFile(path.join(output, 'results.json'), JSON.stringify({ results, fatal }, null, 2));
  await browser.close();
  // The default test path is the current checkout. A remote backend has its
  // own workspace and must clean up its owned test directory on that host.
  if (root.endsWith('/dali-ui')) {
    await fs.rm(path.join(root, 'samples', directory), { recursive: true, force: true });
  }
}
