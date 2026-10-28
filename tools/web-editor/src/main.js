import * as monaco from 'monaco-editor/editor/editor.api.js';
import 'monaco-editor/editor/browser/coreCommands.js';
import 'monaco-editor/editor/contrib/find/browser/findController.js';
import 'monaco-editor/editor/contrib/folding/browser/folding.js';
import 'monaco-editor/editor/contrib/bracketMatching/browser/bracketMatching.js';
import 'monaco-editor/editor/contrib/contextmenu/browser/contextmenu.js';
import 'monaco-editor/editor/contrib/clipboard/browser/clipboard.js';
import 'monaco-editor/editor/contrib/wordOperations/browser/wordOperations.js';
import 'monaco-editor/editor/contrib/indentation/browser/indentation.js';
import 'monaco-editor/languages/definitions/cpp/register.js';
import 'monaco-editor/languages/definitions/python/register.js';
import 'monaco-editor/languages/definitions/shell/register.js';
import 'monaco-editor/languages/definitions/javascript/register.js';
import 'monaco-editor/languages/definitions/xml/register.js';
import EditorWorker from 'monaco-editor/editor/editor.worker.js?worker';
import { Terminal } from '@xterm/xterm';
import { FitAddon } from '@xterm/addon-fit';
import './style.css';

globalThis.MonacoEnvironment = { getWorker: () => new EditorWorker() };
const $ = id => document.getElementById(id);
const icons = {
  'file-plus': '<path d="M13 3H5v18h14V9zM13 3v6h6M8 15h8M12 11v8"/>',
  'folder-plus': '<path d="M3 7V5h6l2 2h10v13H3zM9 14h6M12 11v6"/>',
  save: '<path d="M4 3h13l3 3v15H4zM8 3v6h8V3M8 21v-8h8v8"/>',
  refresh: '<path d="M20 7v5h-5M4 17v-5h5M5.5 7a8 8 0 0 1 13-1L20 9M4 15l1.5 3a8 8 0 0 0 13-1"/>',
  stop: '<rect x="6" y="6" width="12" height="12" rx="1"/>',
  play: '<path d="m7 4 14 8-14 8z"/>',
  panels: '<rect x="3" y="4" width="18" height="16" rx="1"/><path d="M8 4v16M16 4v16M8 14h8"/>',
  monitor: '<rect x="3" y="4" width="18" height="13" rx="2"/><path d="M8 21h8M12 17v4"/>',
  collapse: '<path d="M7 3h14v14M3 7h14v14H3zM6 14h8"/>',
  clear: '<path d="m6 17 10-12 6 5-10 12H7l-5-5 4-4M10 12l6 5M12 22h10"/>',
  code: '<path d="m8 6-6 6 6 6M16 6l6 6-6 6M14 3l-4 18"/>',
};
function icon(name) { return `<svg viewBox="0 0 24 24" aria-hidden="true">${icons[name] || icons.code}</svg>`; }
document.querySelectorAll('[data-icon]').forEach(element => { element.innerHTML = icon(element.dataset.icon); });

const state = { config: null, docs: new Map(), active: null, expanded: new Set(), rows: new Map(),
  build: null, saving: false, socket: null, connected: false, terminal: 'shell',
  scene: [800, 600], previewTarget: null, previewVersion: '', diagnosticList: [] };
const terminals = {};
const fits = {};
let editor;
let toastTimer;
function notify(message, error = false) {
  clearTimeout(toastTimer);
  $('toast').textContent = message;
  $('toast').classList.toggle('error', error);
  $('toast').hidden = false;
  toastTimer = setTimeout(() => { $('toast').hidden = true; }, error ? 11000 : 3500);
}
const task = fn => (...args) => Promise.resolve().then(() => fn(...args)).catch(error => notify(error.message, true));
async function api(endpoint, options = {}) {
  const response = await fetch(`/api/${endpoint}`, { ...options,
    headers: { 'Content-Type': 'application/json', 'X-Dali-Editor-Token': state.config?.token || '', ...options.headers } });
  const data = await response.json();
  if (!response.ok) throw new Error(data.error || `Request failed (${response.status})`);
  return data;
}
const dirty = doc => doc.model.getValue() !== doc.saved;
const busy = () => state.saving || ['building', 'cancelling'].includes(state.build?.status);
function controls() {
  $('build-run').disabled = busy() || !state.connected;
  $('build-run').querySelector('span:nth-child(2)').textContent = state.saving ? 'Saving…' : busy() ? 'Building…' : 'Build & Run';
  $('sample-select').disabled = busy();
  $('save-all').disabled = state.saving || ![...state.docs.values()].some(dirty);
  $('stop').disabled = !busy() && !state.previewTarget;
}
function language(file) {
  const ext = file.split('.').pop().toLowerCase();
  return ({ cpp: 'cpp', cc: 'cpp', c: 'cpp', h: 'cpp', hpp: 'cpp', py: 'python', sh: 'shell',
    js: 'javascript', mjs: 'javascript', json: 'json', xml: 'xml', html: 'xml', md: 'markdown' })[ext] || 'plaintext';
}
function tabs() {
  $('editor-tabs').replaceChildren();
  for (const [file, doc] of state.docs) {
    const tab = document.createElement('div');
    tab.className = `editor-tab${state.active === file ? ' active' : ''}`;
    tab.dataset.path = file;
    tab.setAttribute('role', 'tab');
    tab.setAttribute('aria-selected', String(state.active === file));
    tab.tabIndex = state.active === file ? 0 : -1;
    const symbol = document.createElement('span'); symbol.className = 'file-symbol'; symbol.textContent = language(file) === 'cpp' ? 'C++' : '≡';
    const name = document.createElement('span'); name.textContent = file.split('/').pop();
    tab.append(symbol, name);
    if (dirty(doc)) { const mark = document.createElement('span'); mark.className = 'dirty-marker'; mark.setAttribute('aria-label', 'Unsaved changes'); tab.append(mark); }
    const close = document.createElement('button'); close.className = 'tab-close'; close.textContent = '×';
    close.setAttribute('aria-label', `Close ${name.textContent}`);
    close.onclick = event => { event.stopPropagation(); closeFile(file); };
    tab.append(close); tab.title = file;
    tab.onclick = () => activate(file);
    tab.onkeydown = event => { if (event.key === 'Enter') activate(file); };
    $('editor-tabs').append(tab);
  }
  controls();
}
function activate(file, position) {
  const previous = state.docs.get(state.active);
  if (previous) previous.view = editor.saveViewState();
  state.active = file;
  const doc = state.docs.get(file);
  editor.setModel(doc?.model || null);
  $('editor-empty').hidden = !!doc;
  if (doc?.view) editor.restoreViewState(doc.view);
  if (position) { editor.setPosition(position); editor.revealLineInCenter(position.lineNumber); }
  $('breadcrumbs').textContent = file ? file.split('/').join('  ›  ') : '';
  $('language-status').textContent = doc ? (language(file) === 'cpp' ? 'C++' : language(file)) : '';
  document.title = `${file ? file.split('/').pop() + ' — ' : ''}DALi Editor`;
  document.querySelectorAll('.tree-row').forEach(row => row.classList.toggle('selected', row.dataset.path === file));
  tabs();
}
function closeFile(file) {
  const doc = state.docs.get(file);
  if (dirty(doc) && !confirm(`Discard unsaved changes in ${file.split('/').pop()}?`)) return;
  state.docs.delete(file);
  if (state.active === file) activate([...state.docs.keys()].at(-1) || null);
  else tabs();
  doc.model.dispose();
}
async function openFile(file, position) {
  if (!state.docs.has(file)) {
    const data = await api(`file?path=${encodeURIComponent(file)}`);
    const model = monaco.editor.createModel(data.content, language(file), monaco.Uri.file('/' + file));
    const doc = { model, saved: data.content, revision: data.revision, view: null };
    model.onDidChangeContent(tabs);
    state.docs.set(file, doc);
    applyMarkers(file, doc);
  }
  activate(file, position);
  await revealFile(file);
}
async function saveAll() {
  if (state.saving) throw new Error('A save is already in progress.');
  state.saving = true; controls();
  try {
    let count = 0;
    for (const [file, doc] of state.docs) {
      if (!dirty(doc)) continue;
      const content = doc.model.getValue();
      const result = await api('file', { method: 'PUT', body: JSON.stringify({ path: file, content, revision: doc.revision }) });
      doc.revision = result.revision; doc.saved = content; ++count;
    }
    $('global-status').textContent = count ? `Saved ${count} file${count === 1 ? '' : 's'}` : 'All changes saved';
  } finally { state.saving = false; tabs(); }
}
async function reloadFile() {
  const file = state.active;
  if (!file) return;
  const doc = state.docs.get(file);
  if (dirty(doc) && !confirm(`Discard unsaved changes and reload ${file.split('/').pop()}?`)) return;
  const data = await api(`file?path=${encodeURIComponent(file)}`);
  doc.saved = data.content; doc.revision = data.revision;
  doc.model.setValue(data.content); tabs();
  notify('Reloaded from disk');
}

async function directory(container, relative, depth) {
  const { entries } = await api(`tree?path=${encodeURIComponent(relative)}`);
  container.replaceChildren();
  for (const entry of entries) {
    const item = document.createElement('div'); item.setAttribute('role', 'treeitem');
    const row = document.createElement('button'); row.className = `tree-row ${entry.type}`;
    row.dataset.path = entry.path; row.style.paddingLeft = `${10 + depth * 14}px`;
    row.title = entry.path;
    row.classList.toggle('selected', entry.path === state.active);
    const chevron = document.createElement('span'); chevron.className = 'chevron';
    chevron.textContent = entry.type === 'directory' ? '›' : '';
    const symbol = document.createElement('span'); symbol.className = 'file-symbol';
    symbol.textContent = entry.type === 'directory' ? '▱' : language(entry.name) === 'cpp' ? 'C++' : '≡';
    const label = document.createElement('span'); label.className = 'filename'; label.textContent = entry.name;
    row.append(chevron, symbol, label); item.append(row); container.append(item);
    state.rows.set(entry.path, { row, item, depth, chevron, expanded: false });
    row.onclick = task(async () => {
      if (entry.type === 'file') await openFile(entry.path);
      else await toggleDirectory(entry.path);
    });
    if (entry.type === 'directory' && state.expanded.has(entry.path)) await toggleDirectory(entry.path, true);
  }
}
async function toggleDirectory(relative, force = false) {
  const node = state.rows.get(relative);
  if (!node || (force && node.expanded)) return;
  if (node.expanded) {
    node.item.querySelector('.tree-children')?.remove();
    node.chevron.textContent = '›'; node.expanded = false; state.expanded.delete(relative);
    node.item.setAttribute('aria-expanded', 'false');
  } else {
    const children = document.createElement('div'); children.className = 'tree-children'; children.setAttribute('role', 'group');
    node.item.append(children);
    await directory(children, relative, node.depth + 1);
    node.chevron.textContent = '⌄'; node.expanded = true; state.expanded.add(relative);
    node.item.setAttribute('aria-expanded', 'true');
  }
}
async function refreshTree() { state.rows.clear(); await directory($('file-tree'), '', 0); }
async function revealFile(file) {
  const parts = file.split('/');
  for (let index = 1; index < parts.length; ++index) await toggleDirectory(parts.slice(0, index).join('/'), true);
  const row = state.rows.get(file)?.row;
  row?.classList.add('selected'); row?.scrollIntoView({ block: 'nearest' });
}

function outputTab(name) {
  state.terminal = name;
  document.querySelectorAll('.output-tab').forEach(tab => {
    const selected = tab.dataset.terminal === name; tab.classList.toggle('active', selected); tab.setAttribute('aria-selected', String(selected));
  });
  document.querySelectorAll('.terminal-view').forEach(view => view.classList.toggle('active', view.id === `terminal-${name}`));
  $('terminal-label').textContent = name === 'shell' ? 'bash' : name === 'build' ? 'Emscripten' : 'stdout / stderr';
  fitAll();
}
function send(value) { if (state.socket?.readyState === WebSocket.OPEN) state.socket.send(JSON.stringify(value)); }
function fitAll() {
  editor?.layout();
  for (const [name, fit] of Object.entries(fits)) {
    fit.fit();
    if (name === 'shell') send({ type: 'terminal.resize', cols: terminals.shell.cols, rows: terminals.shell.rows });
  }
}
function setupTerminals() {
  for (const name of ['shell', 'build', 'runtime']) {
    const terminal = new Terminal({ fontFamily: '"DejaVu Sans Mono", "Liberation Mono", monospace', fontSize: 11,
      lineHeight: 1.32, cursorBlink: name === 'shell', disableStdin: name !== 'shell',
      scrollback: 5000, convertEol: true,
      theme: { background: '#11161e', foreground: '#b5c2d4', cursor: '#67d7b1',
        black: '#273345', red: '#f08989', green: '#91cda5', yellow: '#e5ca8b',
        blue: '#7ca8dc', magenta: '#c29ce1', cyan: '#80c9d4', white: '#d7e1ee', selectionBackground: '#34465c' } });
    const fit = new FitAddon(); terminal.loadAddon(fit); terminal.open($(`terminal-${name}`));
    terminals[name] = terminal; fits[name] = fit;
    if (name === 'shell') terminal.onData(data => send({ type: 'terminal.input', data }));
  }
  terminals.build.writeln('\x1b[90mBuild output will appear here. Ctrl+Enter to build and run.\x1b[0m');
  terminals.runtime.writeln('\x1b[90mApplication stdout and stderr will appear here.\x1b[0m');
}
function applyMarkers(file, doc) {
  monaco.editor.setModelMarkers(doc.model, 'dali-build', state.diagnosticList.filter(value => value.path === file).map(value => ({
    startLineNumber: value.line, startColumn: value.column, endLineNumber: value.line, endColumn: value.column + 1,
    message: value.message, severity: value.severity === 'warning' ? monaco.MarkerSeverity.Warning : monaco.MarkerSeverity.Error,
  })));
}
function problems(build) {
  state.diagnosticList = build?.diagnostics || [];
  $('problems').replaceChildren(); $('problems').hidden = !state.diagnosticList.length;
  for (const problem of state.diagnosticList) {
    const button = document.createElement('button'); button.className = 'problem';
    button.textContent = `${problem.severity === 'error' ? '×' : '△'} ${problem.path.split('/').pop()}:${problem.line}  ${problem.message}`;
    button.title = `${problem.path}:${problem.line}:${problem.column}`;
    button.onclick = task(() => openFile(problem.path, { lineNumber: problem.line, column: problem.column }));
    $('problems').append(button);
  }
  for (const [file, doc] of state.docs) applyMarkers(file, doc);
  fitAll();
}
function setBuild(build, replay = false) {
  if (!build) return;
  const changed = state.build?.id !== build.id;
  state.build = build;
  if (changed) { terminals.build.clear(); problems(null); }
  if (changed && replay && build.output) terminals.build.write(build.output);
  $('build-indicator').textContent = busy() ? ' ' : '';
  if (busy()) {
    $('build-status').textContent = build.status === 'cancelling' ? 'Stopping build…' : `Building ${build.target}`;
    $('global-status').textContent = 'Building from workspace sources';
  } else {
    const seconds = ((build.finishedAt - build.startedAt) / 1000).toFixed(1);
    $('build-status').textContent = `${build.status === 'succeeded' ? '✓ Built' : build.status === 'cancelled' ? 'Build stopped' : 'Build failed'} · ${seconds}s`;
    $('global-status').textContent = build.status === 'succeeded' ? 'Build succeeded' : build.status === 'failed' ? 'Build failed · previous preview kept' : 'Build cancelled';
    problems(build);
  }
  controls();
}
function connect() {
  const url = new URL('/ws', location.href); url.protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
  const socket = new WebSocket(url, ['dali-editor', state.config.token]); state.socket = socket;
  socket.onopen = () => {
    state.connected = true; $('connection-dot').classList.add('green'); $('connection-status').textContent = 'Local backend';
    controls(); fitAll();
  };
  socket.onclose = () => {
    state.connected = false; controls(); $('connection-dot').classList.remove('green'); $('connection-status').textContent = 'Disconnected';
    terminals.shell.writeln('\r\n\x1b[90mDisconnected. Reconnecting…\x1b[0m');
    setTimeout(connect, 1800);
  };
  socket.onmessage = event => {
    const value = JSON.parse(event.data);
    if (value.type === 'terminal.output') terminals.shell.write(value.data);
    else if (value.type === 'terminal.ready') fitAll();
    else if (value.type === 'terminal.exit') terminals.shell.writeln(`\r\nShell exited (${value.exitCode}). Use Restart terminal to open another shell.`);
    else if (value.type === 'terminal.error') notify(value.message, true);
    else if (value.type === 'build.started') { setBuild(value.build); outputTab('build'); }
    else if (value.type === 'build.output') terminals.build.write(value.data);
    else if (value.type === 'build.state') setBuild(value.build, true);
    else if (value.type === 'build.finished') {
      setBuild(value.build);
      if (value.build.status === 'succeeded') {
        state.config.readyTargets.push(value.build.target);
        state.config.previewPaths[value.build.target] = value.build.previewPath;
        loadPreview(value.build.target, value.build.id);
      } else if (value.build.status === 'failed') notify('Build failed. Open an error below the build log to jump to its source.', true);
    }
  };
}

function previewState(text, kind = '') { $('preview-state').textContent = text; $('preview-state').className = `state-chip ${kind}`; }
function loadPreview(target = $('sample-select').value, version = String(Date.now())) {
  if (!state.config.readyTargets.includes(target)) { notify('Build this target before starting its preview.'); return; }
  state.previewTarget = target; state.previewVersion = version;
  const artifact = state.config.previewPaths[target] || `${encodeURIComponent(target)}.html`;
  const url = new URL(artifact, state.config.previewOrigin + '/');
  url.search = new URLSearchParams({ embedded: '1', sceneWidth: state.scene[0], sceneHeight: state.scene[1], build: version });
  terminals.runtime.clear();
  terminals.runtime.writeln(`\x1b[36mStarting ${target}\x1b[0m\r\n`);
  $('preview-placeholder').hidden = true; $('preview-frame').hidden = false;
  $('preview-frame').src = url.href;
  $('preview-frame').title = `DALi application preview · ${target}`;
  $('surface-dimensions').textContent = 'SURFACE —'; previewState('Loading…', 'building'); controls();
}
function stopPreview() {
  $('preview-frame').src = 'about:blank'; $('preview-frame').hidden = true; $('preview-placeholder').hidden = false;
  state.previewTarget = null; previewState('Stopped'); controls();
}
async function buildAndRun() {
  if (busy()) return;
  await saveAll();
  await api('builds', { method: 'POST', body: JSON.stringify({ target: $('sample-select').value }) });
}
async function changeSample() {
  const sample = state.config.samples.find(sample => sample.target === $('sample-select').value);
  if (sample?.source) await openFile(sample.source);
  if (state.previewTarget) stopPreview();
  if ($('scene-size').value === 'sample') applySceneSize(...sample.sceneSize);
  if (state.config.readyTargets.includes(sample.target)) loadPreview(sample.target);
}
function applySceneSize(width, height) {
  if (![width, height].every(value => Number.isInteger(value) && value >= 64 && value <= 8192)) throw new Error('Scene dimensions must be integers between 64 and 8192.');
  state.scene = [width, height]; $('scene-dimensions').textContent = `SCENE ${width} × ${height}`;
  if (state.previewTarget) loadPreview(state.previewTarget);
}
window.addEventListener('message', event => {
  if (!state.config || !state.previewTarget || event.origin !== state.config.previewOrigin || event.source !== $('preview-frame').contentWindow
    || event.data?.source !== 'dali-preview') return;
  const value = event.data;
  if (value.type === 'log') terminals.runtime.write(String(value.text) + '\r\n');
  else if (value.type === 'status') {
    if (/runtime ready|running/.test(value.text)) previewState('Running', 'running');
    else if (/Failed:/.test(value.text)) previewState('Runtime error', 'error');
    else if (/exited/.test(value.text)) previewState('Exited');
  } else if (value.type === 'surface') $('surface-dimensions').textContent = `SURFACE ${value.width} × ${value.height}`;
});

function setupLayout() {
  const workbench = $('workbench');
  const reset = () => {
    workbench.style.setProperty('--explorer-width', '218px');
    workbench.style.setProperty('--code-width', `${Math.max(340, Math.round(workbench.clientWidth * .42))}px`);
    $('center-panel').style.setProperty('--preview-height', '62%'); fitAll();
    localStorage.removeItem('dali-editor-layout');
  };
  try {
    const saved = JSON.parse(localStorage.getItem('dali-editor-layout'));
    if (saved) {
      workbench.style.setProperty('--explorer-width', `${Math.max(150, Math.min(350, saved.explorer))}px`);
      workbench.style.setProperty('--code-width', `${Math.max(300, Math.min(workbench.clientWidth - 480, saved.code))}px`);
      $('center-panel').style.setProperty('--preview-height', `${Math.max(30, Math.min(78, saved.preview))}%`);
    }
  } catch {}
  for (const name of ['explorer', 'code', 'terminal']) {
    const handle = $(`${name}-splitter`);
    const resize = event => {
      const bounds = workbench.getBoundingClientRect();
      const explorer = $('explorer-panel').getBoundingClientRect().width;
      const code = $('code-panel').getBoundingClientRect().width;
      if (name === 'explorer') workbench.style.setProperty('--explorer-width', `${Math.max(150, Math.min(bounds.width - code - 290, event.clientX - bounds.left))}px`);
      else if (name === 'code') workbench.style.setProperty('--code-width', `${Math.max(300, Math.min(bounds.width - explorer - 290, bounds.right - event.clientX))}px`);
      else {
        const center = $('center-panel').getBoundingClientRect();
        const height = Math.max(170, Math.min(center.height - 135, event.clientY - center.top));
        $('center-panel').style.setProperty('--preview-height', `${height / center.height * 100}%`);
      }
      fitAll();
    };
    const save = () => localStorage.setItem('dali-editor-layout', JSON.stringify({
      explorer: $('explorer-panel').clientWidth, code: $('code-panel').clientWidth,
      preview: $('preview-panel').clientHeight / $('center-panel').clientHeight * 100,
    }));
    handle.onpointerdown = event => {
      if (event.button !== 0) return;
      event.preventDefault(); handle.setPointerCapture(event.pointerId);
      document.body.classList.add('resizing'); handle.classList.add('dragging');
      handle.onpointermove = resize;
      handle.onpointerup = handle.onpointercancel = () => {
        handle.onpointermove = null; document.body.classList.remove('resizing'); handle.classList.remove('dragging'); save();
      };
    };
    handle.onkeydown = event => {
      if (!['ArrowLeft', 'ArrowRight', 'ArrowUp', 'ArrowDown'].includes(event.key)) return;
      event.preventDefault(); const bounds = handle.getBoundingClientRect();
      const delta = (event.key === 'ArrowLeft' || event.key === 'ArrowUp' ? -1 : 1) * (event.shiftKey ? 20 : 5);
      resize({ clientX: bounds.x + bounds.width / 2 + delta, clientY: bounds.y + bounds.height / 2 + delta }); save();
    };
    handle.ondblclick = reset;
  }
  $('reset-layout').onclick = reset;
  new ResizeObserver(() => requestAnimationFrame(fitAll)).observe(workbench);
  new ResizeObserver(() => requestAnimationFrame(fitAll)).observe($('code-panel'));
  new ResizeObserver(() => requestAnimationFrame(fitAll)).observe($('terminal-content'));
}

function newEntry(type) {
  $('file-dialog-title').textContent = type === 'file' ? 'New file' : 'New folder';
  $('new-path').value = state.active ? state.active.split('/').slice(0, -1).join('/') + '/' : 'dali-ui/samples/';
  $('dialog-error').textContent = ''; $('file-dialog').showModal(); $('new-path').focus();
  $('file-form').onsubmit = async event => {
    event.preventDefault();
    try {
      const result = await api('files', { method: 'POST', body: JSON.stringify({ path: $('new-path').value, type }) });
      $('file-dialog').close(); await refreshTree();
      if (type === 'file') await openFile(result.path);
      notify(type === 'file' ? 'File created' : 'Folder created');
    } catch (error) { $('dialog-error').textContent = error.message; }
  };
}

async function init() {
  state.config = await api('config');
  $('workspace-name').textContent = `${state.config.name} / dali-ui`; $('workspace-name').title = state.config.root;
  $('tree-root').textContent = state.config.name.toUpperCase();
  monaco.editor.defineTheme('dali-dark', { base: 'vs-dark', inherit: true, rules: [], colors: {
    'editor.background': '#171c24', 'editor.foreground': '#c6d1df', 'editorLineNumber.foreground': '#49576a',
    'editorLineNumber.activeForeground': '#9eacbd', 'editor.lineHighlightBackground': '#1d2531',
    'editor.selectionBackground': '#304b65', 'editorCursor.foreground': '#67d7b1', 'editorIndentGuide.background1': '#293341',
    'editorWidget.background': '#212b39', 'editorGutter.background': '#171c24', 'scrollbarSlider.background': '#53627a44',
  } });
  editor = monaco.editor.create($('editor'), { model: null, theme: 'dali-dark', fontSize: 12, lineHeight: 20,
    fontFamily: '"DejaVu Sans Mono", "Liberation Mono", monospace', minimap: { enabled: false },
    scrollBeyondLastLine: false, automaticLayout: false, tabSize: 2, insertSpaces: true, padding: { top: 12 },
    smoothScrolling: true, renderLineHighlight: 'line', bracketPairColorization: { enabled: true },
    experimentalEditContextEnabled: false, ariaLabel: 'Source code editor', fixedOverflowWidgets: true });
  editor.onDidChangeCursorPosition(({ position }) => { $('cursor-position').textContent = `Ln ${position.lineNumber}, Col ${position.column}`; });
  editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS, task(saveAll));
  editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter, task(buildAndRun));
  setupTerminals(); setupLayout();
  $('sample-select').replaceChildren();
  for (const verified of [true, false]) {
    const group = document.createElement('optgroup'); group.label = verified ? 'Verified Web samples' : 'Other samples · platform support varies';
    for (const sample of state.config.samples.filter(sample => sample.verified === verified)) {
      const option = document.createElement('option'); option.value = sample.target; option.textContent = sample.target;
      group.append(option);
    }
    $('sample-select').append(group);
  }
  $('sample-select').value = state.config.defaultTarget;
  $('sample-select').onchange = task(changeSample);
  $('save-all').onclick = task(saveAll); $('build-run').onclick = task(buildAndRun);
  $('reload-file').onclick = task(reloadFile); $('reload-preview').onclick = () => loadPreview();
  $('stop').onclick = task(async () => { if (busy()) await api('builds/current', { method: 'DELETE' }); stopPreview(); });
  $('new-file').onclick = () => newEntry('file'); $('new-folder').onclick = () => newEntry('directory');
  $('cancel-dialog').onclick = () => $('file-dialog').close();
  $('refresh-tree').onclick = task(refreshTree);
  $('collapse-tree').onclick = task(async () => { state.expanded.clear(); await refreshTree(); });
  document.querySelectorAll('[data-terminal]').forEach(button => { button.onclick = () => outputTab(button.dataset.terminal); });
  $('clear-terminal').onclick = () => terminals[state.terminal].clear();
  $('restart-terminal').onclick = () => { terminals.shell.clear(); outputTab('shell'); send({ type: 'terminal.restart' }); };
  $('scene-size').onchange = task(() => {
    const custom = $('scene-size').value === 'custom'; $('custom-size').hidden = !custom;
    if (!custom) {
      const sample = state.config.samples.find(sample => sample.target === $('sample-select').value);
      applySceneSize(...($('scene-size').value === 'sample' ? sample.sceneSize : $('scene-size').value.split(',').map(Number)));
    }
  });
  $('apply-size').onclick = task(() => applySceneSize(Number($('scene-width').value), Number($('scene-height').value)));
  window.addEventListener('beforeunload', event => { if ([...state.docs.values()].some(dirty)) { event.preventDefault(); event.returnValue = ''; } });
  window.addEventListener('keydown', event => {
    if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === 's') { event.preventDefault(); if (!editor.hasTextFocus()) task(saveAll)(); }
    if ((event.ctrlKey || event.metaKey) && event.key === 'Enter') { event.preventDefault(); if (!editor.hasTextFocus()) task(buildAndRun)(); }
  });
  connect();
  await refreshTree(); await changeSample();
  if (state.config.build) setBuild(state.config.build, true);
  $('global-status').textContent = 'Workspace ready'; controls(); fitAll();
}

init().catch(error => { $('global-status').textContent = 'Could not open workspace'; notify(error.message, true); });
