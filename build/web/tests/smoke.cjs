// Run against the provided sample server. Uses a disposable browser profile.
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { chromium } = require('playwright-core');
const { PNG } = require('pngjs');

const base = process.env.DALI_WEB_TEST_URL || 'http://127.0.0.1:8765/';
const output = process.env.DALI_WEB_TEST_OUTPUT || '/tmp/dali-web-test-results';
const executable = process.env.DALI_WEB_TEST_BROWSER || '/usr/bin/google-chrome';
const samples = ['hello-world', 'animation', 'text', 'image-view', 'stacklayout',
  'flexlayout', 'gridlayout', 'scrollview', 'key-input', 'absolutelayout'];
fs.mkdirSync(output, { recursive: true });

function pixel(buffer, x, y) {
  const png = PNG.sync.read(buffer);
  const offset = (y * png.width + x) * 4;
  return [...png.data.subarray(offset, offset + 3)];
}
function changed(before, after) {
  const a = PNG.sync.read(before).data;
  const b = PNG.sync.read(after).data;
  let count = 0;
  for (let i = 0; i < a.length; i += 4) {
    if (a[i] !== b[i] || a[i + 1] !== b[i + 1] || a[i + 2] !== b[i + 2]) ++count;
  }
  return count;
}
async function frame(page) { return page.locator('canvas').screenshot(); }
async function waitForPixels(page, condition) {
  const deadline = Date.now() + 30000;
  while (Date.now() < deadline) {
    const image = await frame(page);
    if (condition(image)) return image;
    await page.waitForTimeout(150);
  }
  throw new Error('Timed out waiting for the expected DALi frame');
}

(async () => {
  const browser = await chromium.launch({ executablePath: executable, headless: true,
    args: ['--no-sandbox', '--use-angle=swiftshader', '--enable-unsafe-swiftshader'] });
  const context = await browser.newContext({ viewport: { width: 1280, height: 1440 } });
  // Exercise Web fetch deterministically, independently of external network availability.
  const fixture = fs.readFileSync(path.resolve(__dirname, '../../../samples/image-view/res/gallery-large-3.jpg'));
  await context.route('https://raw.githubusercontent.com/dalihub/dali-test/**', route => route.fulfill({
    status: 200, contentType: 'image/jpeg', body: fixture,
    headers: { 'access-control-allow-origin': '*', 'cross-origin-resource-policy': 'cross-origin' }
  }));
  const results = [];
  try {
    for (const name of samples) {
      const page = await context.newPage();
      const logs = [];
      const fatal = [];
      const logPath = path.join(output, `${name}.log`);
      fs.writeFileSync(logPath, '');
      page.on('console', message => {
        const text = message.text(); logs.push(`${message.type()}: ${text}`);
        fs.appendFileSync(logPath, `${message.type()}: ${text}\n`);
        if (/WebGL: INVALID_|glMapBufferRange access|Aborted\(|Assertion failed|RuntimeError:|undefined symbol|Load failed|dlopen error|Fail to read file/.test(text)) fatal.push(text);
      });
      page.on('pageerror', error => { logs.push(error.stack); fatal.push(error.message); fs.appendFileSync(logPath, error.stack + '\n'); });
      await page.goto(new URL(`${name}.example.html`, base).href);
      assert.equal(await page.evaluate(() => crossOriginIsolated), true, 'COOP/COEP headers missing');
      await page.waitForFunction(() => /DALi (running|runtime ready)/.test(document.querySelector('#status').textContent));
      const before = await waitForPixels(page, image => {
        const colors = new Set(); const rgba = PNG.sync.read(image).data;
        for (let i = 0; i < rgba.length; i += 4) colors.add(rgba.readUInt32BE(i));
        return colors.size > 2;
      });
      fs.writeFileSync(path.join(output, `${name}.png`), before);
      if (name === 'absolutelayout') {
        assert.deepEqual(pixel(before, 75, 75), [255, 0, 0]);
        assert.deepEqual(pixel(before, 150, 150), [0, 255, 0]);
        assert.deepEqual(pixel(before, 250, 225), [0, 0, 255]);
      }
      if (name === 'hello-world') {
        assert.deepEqual(pixel(before, 50, 50), [0, 0, 255]);
        await page.locator('canvas').click({ position: { x: 50, y: 50 } });
        const after = await waitForPixels(page, image => pixel(image, 50, 50)[1] > 240);
        fs.writeFileSync(path.join(output, 'hello-world-click.png'), after);
      }
      if (name === 'animation') {
        await page.locator('canvas').click({ position: { x: 50, y: 50 } });
        const after = await waitForPixels(page, image => changed(before, image) > 2000);
        fs.writeFileSync(path.join(output, 'animation-playing.png'), after);
        await page.waitForTimeout(800);
        assert.deepEqual(pixel(await frame(page), 50, 5), [255, 255, 255], 'PositionY animation did not finish');
      }
      if (name === 'image-view') {
        const deadline = Date.now() + 15000;
        while (Date.now() < deadline && !logs.some(text => /url=https:.*status=1/.test(text))) await page.waitForTimeout(100);
        assert(logs.some(text => /url=\/res\/.*status=1/.test(text)), 'Local image did not load');
        assert(logs.some(text => /url=https:.*status=1/.test(text)), 'Web fetch image did not load');
      }
      if (name === 'key-input') {
        await page.locator('canvas').focus();
        await page.keyboard.press('a');
        await page.keyboard.press('ArrowRight');
        const after = await waitForPixels(page, image => changed(before, image) > 1000);
        fs.writeFileSync(path.join(output, 'key-input-events.png'), after);
      }
      if (name === 'scrollview') {
        const bounds = await page.locator('canvas').boundingBox();
        await page.mouse.move(bounds.x + 300, bounds.y + 650);
        await page.mouse.down();
        await page.mouse.move(bounds.x + 300, bounds.y + 500, { steps: 10 });
        await page.mouse.up();
        const after = await waitForPixels(page, image => changed(before, image) > 1000);
        fs.writeFileSync(path.join(output, 'scrollview-drag.png'), after);
      }
      if (name === 'image-view') {
        await page.locator('canvas').focus();
        await page.keyboard.press('Escape');
        await page.waitForFunction(() => document.querySelector('#status').textContent === 'DALi exited (0)');
      }
      assert.deepEqual(fatal, [], `${name}: browser or GL failure`);
      fs.writeFileSync(path.join(output, `${name}.log`), logs.join('\n'));
      results.push({ sample: name, passed: true });
      console.log(`PASS ${name}`);
      await page.close();
    }
  } finally {
    fs.writeFileSync(path.join(output, 'results.json'), JSON.stringify(results, null, 2));
    await browser.close();
  }
})().catch(error => { console.error(error); process.exitCode = 1; });
