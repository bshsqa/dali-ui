// Capture only a disposable browser context. No source edits or editor builds.
// Run with the Node.js provided by the DALi Web SDK from the workspace root.
import { createRequire } from 'node:module';
import { fileURLToPath } from 'node:url';
import path from 'node:path';
import { promises as fs } from 'node:fs';
import { createHash } from 'node:crypto';

const output = path.dirname(fileURLToPath(import.meta.url));
let workspace = output;
while (true) {
  try {
    await fs.access(path.join(workspace, 'dali-core/build/web/CMakeLists.txt'));
    await fs.access(path.join(workspace, 'dali-ui/tools/web-editor/package.json'));
    break;
  } catch {
    const parent = path.dirname(workspace);
    if (parent === workspace) throw new Error('Run this script inside a DALi workspace containing the three repositories.');
    workspace = parent;
  }
}
const require = createRequire(path.join(workspace, 'dali-ui/tools/web-editor/package.json'));
const { chromium } = require('playwright-core');
const { PNG } = require('pngjs');
const browser = await chromium.launch({ executablePath: '/usr/bin/google-chrome', headless: true,
  args: ['--no-sandbox', '--use-angle=swiftshader', '--enable-unsafe-swiftshader'] });
const errors = [];
const source = await fs.readFile(path.join(workspace, 'dali-ui/samples/background-blur-dither/background-blur-dither-example.cpp'));
const initialRadius = Number(source.toString().match(/\bBLUR_RADIUS\s*=\s*(\d+)u/)?.[1]);
if (!Number.isFinite(initialRadius)) throw new Error('Cannot determine the sample blur radius.');
try {
  const page = await browser.newPage({ viewport: { width: 1920, height: 1200 }, deviceScaleFactor: 1 });
  page.on('pageerror', error => errors.push(error.message));
  page.on('console', message => {
    if (/WebGL: INVALID_|Aborted\(|RuntimeError:|Assertion failed/.test(message.text())) errors.push(message.text());
  });
  await page.goto('http://127.0.0.1:8787/');
  await page.locator('.monaco-editor').waitFor();
  await page.getByLabel('Application target').selectOption('background-blur-dither.example');
  await page.locator('.editor-tab.active', { hasText: 'background-blur-dither-example.cpp' }).waitFor();
  // Arrange panels using the real editor controls.
  for (const [selector, dx, dy] of [['#code-splitter', 170, 0], ['#terminal-splitter', 0, 115]]) {
    const box = await page.locator(selector).boundingBox();
    const x = box.x + box.width / 2, y = box.y + box.height / 2;
    await page.mouse.move(x, y); await page.mouse.down();
    await page.mouse.move(x + dx, y + dy, { steps: 12 }); await page.mouse.up();
  }
  const canvas = page.frameLocator('#preview-frame').locator('canvas');
  await canvas.waitFor();
  await page.waitForTimeout(7000);
  const logicalSize = await canvas.evaluate(element => [element.width, element.height]);
  await page.getByLabel('Logical scene size').selectOption('custom');
  await page.getByLabel('Scene width').fill(String(logicalSize[0]));
  await page.getByLabel('Scene height').fill(String(logicalSize[1]));
  await page.getByRole('button', { name: 'Apply', exact: true }).click();
  await page.waitForTimeout(5000);
  await canvas.click();
  // Adjust only the running sample's radius for a readable illustration.
  for (let radius = initialRadius; radius > 30; radius -= 10) await page.keyboard.press('3');
  await page.waitForTimeout(1200);
  // Show the actual sample code, without changing it.
  await page.locator('#editor .monaco-editor').click({ position: { x: 130, y: 100 } });
  await page.keyboard.press('Control+Home');
  const editorBox = await page.locator('#editor .monaco-editor').boundingBox();
  await page.mouse.move(editorBox.x + 200, editorBox.y + 250);
  await page.mouse.wheel(0, 2200);
  await page.waitForTimeout(400);
  // A terminal command supplies real, useful context for the screenshot.
  await page.locator('#terminal-shell .xterm-helper-textarea').focus();
  await page.keyboard.type('git -C dali-ui log -1 --format="%h %s"'); await page.keyboard.press('Enter');
  await page.waitForTimeout(600);
  await page.screenshot({ path: path.join(output, '02-editor-poc.png') });
  await canvas.screenshot({ path: path.join(output, '02-preview-blur.png') });
  const before = PNG.sync.read(await canvas.screenshot());
  await canvas.click(); await page.keyboard.press('1');
  await page.waitForTimeout(1100);
  const after = PNG.sync.read(await canvas.screenshot({ path: path.join(output, '02-preview-clear.png') }));
  let changed = 0;
  for (let i = 0; i < before.data.length && i < after.data.length; i += 4) {
    if (Math.abs(before.data[i] - after.data[i]) + Math.abs(before.data[i + 1] - after.data[i + 1])
      + Math.abs(before.data[i + 2] - after.data[i + 2]) > 30) ++changed;
  }
  if (changed < 200) throw new Error('Blur toggle did not visibly change the rendered output.');
  await page.keyboard.press('1');
  await page.waitForTimeout(700);
  if (errors.length) throw new Error(errors.join('\n'));
  const metadata = { capturedFor: '2026-10-28 제안 계획서', classification: 'PoC 참고 화면; 계획 기능의 구현 완료 증거가 아님',
    target: 'background-blur-dither.example', url: 'http://127.0.0.1:8787/',
    viewport: [1920, 1200], renderer: 'headless Chrome / SwiftShader / WebGL 2',
    source: 'dali-ui/samples/background-blur-dither/background-blur-dither-example.cpp',
    sourceSha256: createHash('sha256').update(source).digest('hex'),
    sourcePolicy: '기존 작업 중인 소스를 그대로 사용. 파일 수정 없음.',
    captureSettings: { logicalSize, radius: 30, note: '키 입력으로 실행 중인 샘플의 반경만 조절' },
    blurToggleChangedPixels: changed, runtimeErrors: errors,
    preview: await canvas.evaluate(element => ({ surface: [element.width, element.height],
      isolated: element.ownerDocument.defaultView.crossOriginIsolated })) };
  await fs.writeFile(path.join(output, 'capture-metadata.json'), JSON.stringify(metadata, null, 2) + '\n');
  console.log(JSON.stringify(metadata));
} finally { await browser.close(); }
