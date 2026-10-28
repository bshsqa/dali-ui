import { createRequire } from 'node:module';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { promises as fs } from 'node:fs';
import path from 'node:path';
const folder = path.dirname(fileURLToPath(import.meta.url));
let workspace = folder;
while (true) {
  try { await fs.access(path.join(workspace, 'dali-ui/tools/web-editor/package.json')); break; }
  catch {
    const parent = path.dirname(workspace);
    if (parent === workspace) throw new Error('DALi workspace not found.');
    workspace = parent;
  }
}
const require = createRequire(path.join(workspace, 'dali-ui/tools/web-editor/package.json'));
const { chromium } = require('playwright-core');
const browser = await chromium.launch({ executablePath: '/usr/bin/google-chrome', headless: true, args: ['--no-sandbox'] });
try {
  for (const file of (await fs.readdir(folder)).filter(file => file.endsWith('.svg')).sort()) {
    const source = await fs.readFile(path.join(folder, file), 'utf8');
    const [, width, height] = source.match(/width="(\d+)" height="(\d+)"/);
    const page = await browser.newPage({ viewport: { width: Number(width), height: Number(height) }, deviceScaleFactor: 1 });
    await page.goto(pathToFileURL(path.join(folder, file)).href);
    await page.evaluate(() => document.fonts.ready);
    await page.locator('svg').screenshot({ path: path.join(folder, file.replace('.svg', '.png')) });
    console.log(file);
    await page.close();
  }
} finally { await browser.close(); }
