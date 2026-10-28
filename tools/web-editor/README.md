# DALi App Editor — Ubuntu 로컬 프로토타입

실제 로컬 C++ 파일을 편집하고, 백엔드에서 WASM으로 빌드한 결과를 브라우저에서
실행하는 앱 에디터입니다. 드래그앤드롭 Scene 저작 기능은 아직 포함하지 않습니다.

라이브러리별·샘플별 빌드 명령은 저장소에 포함된
[WEB_BUILD_GUIDE.md](../../docs/web/WEB_BUILD_GUIDE.md)에 정리했습니다.

## 실행

`dali-core`, `dali-adaptor`, `dali-ui`가 같은 부모 디렉터리에 있어야 합니다.
워크스페이스 루트에서 다음 명령을 실행합니다.

```sh
make -C dali-ui/build/web editor
```

브라우저에서 **http://127.0.0.1:8787/** 를 엽니다. 종료는 터미널에서 `Ctrl+C`입니다.
첫 실행 시 잠긴 npm 의존성을 설치하고 프론트엔드를 빌드합니다.
기존 DALi Web SDK가 있으면 그 안의 Node.js를 재사용합니다. Node.js 22.12 이상이
없으면 Web SDK setup을 실행하여 전용 런타임과 빌드 의존성을 준비합니다.
시스템 도구 준비 방법은 [Web backend 안내](../../../dali-adaptor/build/web/README.md)를
참고하십시오. Ubuntu의 `g++`, `make`, Python은 node-pty 빌드에도 필요합니다.

처음에는 `text-gradient.example`을 선택하고 그 소스 파일을 엽니다.
미리 빌드된 결과가 있으면 그라데이션 텍스트 샘플을 바로 표시합니다. 아직 결과물이 없으면
상단의 **Build & Run**을 누릅니다. 이 버튼은 필요한 Web SDK 설치와 DALi 라이브러리
빌드도 수행하므로, 첫 빌드에는 인터넷과 시간이 필요합니다.

포트나 워크스페이스를 바꾸려면 다음처럼 실행합니다.

```sh
bash dali-ui/tools/web-editor/start.sh \
  --workspace /path/to/DALi --port 8790 --preview-port 8791
```

`--workspace`는 세 저장소가 들어 있는 부모 디렉터리입니다. `DALI_EDITOR_WORKSPACE`
환경변수도 사용할 수 있습니다. `DALI_WEB_PREFIX`, `DALI_WEB_BUILD`,
`DALI_WEB_CACHE`, `DALI_WEB_EMSDK`, `DALI_WEB_FONT_DIR`, `JOBS` 설정은 기존 Web
빌드 스크립트와 같습니다. 기본 결과물은 `/tmp/dali-web-build/samples/dist/`입니다.

## 화면과 동작

```text
┌──────────────────────────────────────────────────────────┐
│ New file · Save all · Application · Stop · Build & Run   │
├─────────────┬───────────────────────┬────────────────────┤
│ Explorer    │ Preview               │ Code               │
│             │                       │                    │
│             ├───────────────────────┤                    │
│             │ Terminal / Build /    │                    │
│             │ Application logs      │                    │
└─────────────┴───────────────────────┴────────────────────┘
```

- 세 separator를 마우스로 드래그하거나 방향키로 조절합니다. 크기는 브라우저에
  저장됩니다. 상단 Reset panel sizes 또는 separator 더블클릭으로 초기화합니다.
- Explorer는 실제 디스크 파일을 지연 로딩합니다. 파일 클릭으로 탭을 열고,
  New file / New folder로 워크스페이스 안에 생성합니다. `.git`, `node_modules`,
  생성된 `dist` 등은 제외합니다.
- 코드는 Monaco Editor에서 편집합니다. Save all / `Ctrl+S`는 디스크에 저장하며,
  외부에서 파일이 바뀌면 저장을 거절하여 충돌을 알립니다. Reload active file로
  디스크의 최신 내용을 다시 읽을 수 있습니다.
- **Build & Run / `Ctrl+Enter`**는 변경된 탭을 모두 저장하고,
  `build.sh all --samples <directory> --targets <target>`을 실행합니다. core →
  adaptor → ui → 선택한 앱 순서로 증분 빌드하므로 DALi 소스 변경도 반영됩니다.
- 성공하면 새 iframe으로 결과물을 실행합니다. 컴파일 오류가 있으면 이전 프리뷰를
  유지합니다. Build 탭 아래 오류 항목을 클릭하면 해당 파일·행으로 이동합니다.
  성공 결과는 `$DALI_WEB_BUILD/editor-runs/<build-id>/`에 복사한 뒤 공개하므로
  다음 빌드·취소가 실행 중인 결과물을 덮어쓰지 않습니다. 프로토타입에서는 이
  디렉터리를 자동 삭제하지 않습니다. 에디터와 프리뷰 종료 후 정리할 수 있습니다.
- **Stop**은 실행 중인 빌드 프로세스 그룹을 종료하고 프리뷰 iframe을 제거합니다.
  Preview의 Restart는 마지막 성공 결과를 재실행합니다.
- Terminal은 실제 `/bin/bash` PTY입니다. `git`, `make`, `emcc`와 설치된 CLI를
  입력할 수 있습니다. Build는 빌드 stdout/stderr, Application은 앱 stdout/stderr입니다.
  각 브라우저 탭은 별도 셸을 가지며 연결 종료 시 해당 셸을 정리합니다.
- Application 목록의 Verified Web samples는 기존 검증 대상 10개와 기본 앱인
  `text-gradient.example`입니다. 기본 앱은 초기 화면과 마우스 조작·키 입력·리사이즈를
  확인했으며, 모든 텍스트 효과·16개 case 전체를 검증한 것은 아닙니다.
  다른 타깃도 선택할 수 있지만 네이티브 기능·Lottie 등 지원 범위는
  [Web backend 안내](../../../dali-adaptor/build/web/README.md)를 따릅니다.
  목록은 현재 저장소의 직접 `DALI_ADD_SAMPLE` 선언과 짝지어진 `*_SAMPLES` /
  `*_SOURCES` 목록에서 읽습니다. 일반적인 임의 CMake 프로젝트를 가져오는 기능은 없습니다.

## 고정 Scene과 프리뷰 리사이즈

아래 고정 Scene 변경이 Web 커밋의 선행 이력에 포함되어 있습니다.

- core: `refs/changes/35/353635/6` — 커밋 `2ef0fc27d`
- adaptor: `refs/changes/34/353634/6` — 커밋 `0e8aeb7b0`

사용자가 지정한 patch set 5를 적용할 때 visibility 처리와 최대 프레임레이트 옵션을
유지하면서 충돌을 해결한 결과를 patch set 6으로 올렸습니다. 재현 시에는
[PoC 버전 받기](../../docs/web/README.md)의 최상위 Web 커밋을 체크아웃하면 됩니다.
이 두 요청 패치를 제외한 에디터 구현은 Web 전용 경로에 둡니다.

프리뷰는 `DALI_SCENE_SIZE_FOLLOWS_WINDOW=0`, `DALI_SCENE_WIDTH`,
`DALI_SCENE_HEIGHT`를 사용합니다. Scene의 **App default**는 샘플을 바꿀 때
해당 앱의 기본 논리 크기를 선택합니다. `text-gradient`는 **920 × 880**,
다른 샘플은 **800 × 600**입니다.
Scene 선택에서 Desktop / Portrait / TV / custom 크기를 직접 설정할 수도 있습니다.
논리 크기를 바꾸면 앱을 재실행하며 C++ 재빌드는 하지 않습니다.

패널 크기만 바꾸면 실행 중인 앱을 유지하면서 실제 canvas·render surface를
변경합니다. 논리 Scene은 유지되고 aspect fit으로 그려집니다. 입력 좌표는
브라우저 CSS 좌표 → 실제 surface → 논리 Scene 순서로 변환합니다.
하단의 SCENE과 SURFACE 표시로 두 크기를 구분할 수 있습니다.
앱이 초기화 중 직접 창 크기를 요청하는 경우에도 embedded 프리뷰에서는
호스트 패널 크기로 surface를 다시 맞춰 화면이 비율과 다르게 늘어나지 않게 합니다.

Web의 `WindowBase::ResizeWindow`는 기존 surface 경로에서 render thread가
canvas를 변경하도록 구현했습니다. 같은 크기를 다시 설정해 drawing buffer를
지우거나 render thread에서 창 크기 이벤트를 재진입시키지 않습니다.
`?embedded=1`을 사용하지 않는 기존 독립 샘플의 기본 동작은 유지합니다.

## 프론트엔드와 백엔드 분리

프론트엔드는 상대 경로의 HTTP API와 WebSocket만 사용합니다. 파일과 셸·빌드는
Node.js 백엔드가 실행하고, DALi 렌더링은 사용자 브라우저에서 실행합니다.
현재는 Ubuntu의 신뢰하는 사용자 한 명을 위한 로컬 프로토타입입니다.

| 경로 | 용도 |
| --- | --- |
| `GET /api/config` | 워크스페이스·타깃·프리뷰 주소·세션 정보 |
| `GET /api/tree?path=...` | 디렉터리 조회 |
| `GET /api/file?path=...` | 텍스트와 revision 조회 |
| `PUT /api/file` | revision 확인 후 원자적 파일 저장 |
| `POST /api/files` | 파일·폴더 생성 |
| `POST /api/builds` | 타깃 빌드 시작 |
| `GET /api/builds` | 빌드 상태·로그·진단 |
| `DELETE /api/builds/current` | 빌드 취소 |
| `/ws` | 터미널 입출력·resize와 빌드 상태·출력 스트림 |

편집기(8787)와 프리뷰(8788)는 별도 origin입니다. 프리뷰 서버에는 빌드 결과를
읽는 기능만 있으며 파일 편집·터미널 API는 없습니다. COOP/COEP, 프리뷰 CORP,
iframe의 cross-origin-isolated 권한을 설정하여 현재 WASM pthread 구성을 유지합니다.
편집기 API·WebSocket에는 세션 토큰과 Origin 검사를 적용합니다.

서버 배포 시 같은 API 구조를 재사용할 수 있습니다. reverse proxy의 편집기·프리뷰
주소는 `--origin`, `--preview-origin`으로 지정하고 필요한 헤더를 유지해야 합니다.
다중 사용자 인증, 사용자별 작업 공간·빌드·터미널 격리는 추가 구현이 필요합니다.
서버형의 파일 작업 대상은 서버 워크스페이스이므로 사용자 PC 파일을 쓰려면
업로드·동기화 또는 공유 디렉터리 연결도 필요합니다.

## 개발과 검증

```sh
# 이 디렉터리에서, 적합한 Node.js가 PATH에 있는 상태
npm ci
npm run build
npm start
```

실행 중인 에디터를 대상으로 다음 테스트를 수행할 수 있습니다.

```sh
npm test
npm run test:browser
```

브라우저 테스트에는 로컬 Chrome이 필요합니다. `DALI_WEB_TEST_BROWSER`로
실행 파일을, `DALI_EDITOR_TEST_URL`로 테스트 주소를 지정할 수 있습니다.
headless Chrome의 SwiftShader와 임시 프로필을 사용합니다.
테스트는 자신만의 임시 sample을 생성해 실제 C++ 수정·저장·Emscripten 빌드·
실행, 오류 처리와 취소까지 검사하고 그 sample을 정리합니다. 기존 샘플 소스는
수정하지 않습니다. 결과와 스크린샷은 `/tmp/dali-editor-test-results/`에 저장됩니다.

2026-10-22 Ubuntu 검증 결과:

- 기본 그라데이션 앱을 포함한 에디터 브라우저 흐름 9개와 백엔드 테스트 3개 통과
- 기존 기본 Web 샘플 10개 빌드·브라우저 테스트 통과
- 요청 패치의 core FixedScene/FboTouch 21개, adaptor EnvironmentOptions/SceneHolder 4개 통과
- core → adaptor → ui의 기존 Ubuntu 네이티브 빌드 통과

네이티브 UTC는 별도 `/tmp/dali-editor-debug-prefix`의 Debug/EXPORTALL 라이브러리로
실행했습니다. Web 빌드의 prefix나 기존 네이티브 빌드 설정과 공유하지 않습니다.

현재 C++ 지원은 구문 강조·일반 편집과 빌드 진단입니다. clangd 기반 자동완성,
디버거·중단점, Git 전용 UI, 임의 프로젝트 가져오기, Scene 저작 기능은 포함하지 않습니다.
