#!/usr/bin/env bash
set -euo pipefail
EDITOR_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DALI_WORKSPACE="$(cd "$EDITOR_ROOT/../../.." && pwd)"
DALI_EDITOR_SDK="${DALI_WEB_EMSDK:-${DALI_WEB_CACHE:-$HOME/.cache/dali-web}/emsdk}"
if [[ -f "$DALI_EDITOR_SDK/emsdk_env.sh" ]]; then
  # shellcheck source=/dev/null
  source "$DALI_EDITOR_SDK/emsdk_env.sh" >/dev/null 2>&1
fi
if ! command -v node >/dev/null || ! node -e 'const [a,b]=process.versions.node.split(".").map(Number); process.exit(a>22 || a===22 && b>=12 ? 0 : 1)'; then
  echo "Preparing the private Web SDK and its Node.js runtime…"
  bash "$DALI_WORKSPACE/dali-adaptor/build/web/build.sh" setup
  # shellcheck source=/dev/null
  source "$DALI_EDITOR_SDK/emsdk_env.sh" >/dev/null 2>&1
fi
cd "$EDITOR_ROOT"
mkdir -p .local
# Reuse bundled Node headers when available, avoiding a second header download
# for node-pty's native module. Standard Node installs can let node-gyp fetch them.
DALI_NODE_ROOT="$(node -p 'require("node:path").resolve(require("node:path").dirname(process.execPath), "..")')"
if [[ -f "$DALI_NODE_ROOT/include/node/node.h" ]]; then export npm_package_config_node_gyp_nodedir="$DALI_NODE_ROOT"; fi
DALI_EDITOR_DEPENDENCIES="$(sha256sum package-lock.json | cut -d ' ' -f 1)-$(node -p 'process.versions.modules')"
if [[ ! -f .local/dependencies ]] || [[ "$(cat .local/dependencies)" != "$DALI_EDITOR_DEPENDENCIES" ]] ||
   ! node --input-type=module -e 'await import("node-pty"); await import("vite")' >/dev/null 2>&1; then
  npm ci
  printf '%s\n' "$DALI_EDITOR_DEPENDENCIES" > .local/dependencies
fi
DALI_EDITOR_FRONTEND="$(python3 - <<'PY'
from pathlib import Path
import hashlib
digest = hashlib.sha256()
for file in [Path('index.html'), Path('vite.config.js'), Path('package-lock.json'), *sorted(Path('src').rglob('*'))]:
    if file.is_file():
        digest.update(str(file).encode())
        digest.update(file.read_bytes())
print(digest.hexdigest())
PY
)"
if [[ ! -f dist/index.html ]] || [[ ! -f .local/frontend ]] || [[ "$(cat .local/frontend)" != "$DALI_EDITOR_FRONTEND" ]]; then
  npm run build
  printf '%s\n' "$DALI_EDITOR_FRONTEND" > .local/frontend
fi
exec node server/index.mjs "$@"
