# DALi Web 빌드·실행 가이드

Ubuntu에서 `dali-core`, `dali-adaptor`, `dali-ui`, `dali-ui/samples`를
WASM + WebGL 2로 빌드하는 명령입니다. 아래 명령은 모두 **DALi 폴더에서** 실행합니다.

```sh
cd /path/to/DALi
```

## 1. 처음 전체 빌드하고 에디터 열기

```sh
make -C dali-adaptor/build/web world
make -C dali-ui/build/web editor
```

브라우저에서 **http://127.0.0.1:8787/** 를 엽니다.
기본 앱은 `dali-ui/samples/text/text-gradient-example.cpp`의
**text-gradient.example**입니다. 그라데이션 텍스트와 조작 버튼이 있는 화면을
프리뷰에 띄우고, 오른쪽에는 해당 C++ 파일을 엽니다.

에디터는 터미널에서 계속 실행됩니다. 종료는 `Ctrl+C`입니다.
다음부터 에디터만 열 때는 두 번째 명령만 실행하면 됩니다.

`world`는 외부 의존성 → core → adaptor → ui → 기본 샘플 11개를 순서대로
빌드합니다. 첫 실행에는 SDK·라이브러리·폰트를 내려받으므로 인터넷과 시간이
필요합니다. 이후 같은 명령은 변경된 부분만 빌드합니다.
Emscripten 환경 설정과 CMake 설정은 명령이 자동으로 처리합니다.

새 Ubuntu 환경에서 시스템 도구가 없다면 먼저 한 번 설치합니다.
Python 3.12 이상, CMake 3.24 이상이 필요합니다.

```sh
sudo apt-get install build-essential cmake git python3 python3-pip pkg-config gettext gperf
```

## 2. 라이브러리 하나만 빌드하기

처음 한 번은 위 `world`를 완료한 상태를 기준으로 합니다.
아래에서 필요한 명령만 실행합니다.

**dali-core만:**

```sh
make -C dali-core/build/web install
```

**dali-adaptor만:**

```sh
make -C dali-adaptor/build/web install
```

**dali-ui만:**

```sh
make -C dali-ui/build/web install
```

각 명령은 해당 라이브러리를 빌드하고 Web prefix에 설치합니다.
다른 DALi 라이브러리나 앱까지 자동으로 빌드하지는 않습니다.
앱은 라이브러리를 정적으로 링크하므로, 변경한 라이브러리를 앱에 반영하려면
마지막에 샘플도 다시 빌드해야 합니다.

헤더/API 변경으로 다른 라이브러리에도 영향이 있으면 다음 순서를 따릅니다.

| 변경 위치 | 앱에 반영하는 빌드 순서 |
| --- | --- |
| core | core → adaptor → ui → 샘플 |
| adaptor | adaptor → ui → 샘플 |
| ui | ui → 샘플 |
| 샘플 C++ / 리소스 | 해당 샘플 |

순서를 신경 쓰지 않고 **기본 앱 하나까지** 반영하려면 다음 한 명령을 사용합니다.
필요한 의존성을 순서대로 증분 빌드합니다.

```sh
make -C dali-adaptor/build/web world \
  DALI_WEB_SAMPLE_DIRS=text \
  DALI_WEB_SAMPLE_TARGETS=text-gradient.example
```

에디터의 **Build & Run** 버튼도 선택한 앱에 대해 같은 순서로 빌드·실행합니다.

## 3. 원하는 샘플만 빌드하기

**기본 그라데이션 앱만:**

```sh
make -C dali-ui/build/web/samples \
  DALI_WEB_SAMPLE_DIRS=text \
  DALI_WEB_SAMPLE_TARGETS=text-gradient.example
```

**hello-world만:**

```sh
make -C dali-ui/build/web/samples \
  DALI_WEB_SAMPLE_DIRS=hello-world \
  DALI_WEB_SAMPLE_TARGETS=hello-world.example
```

**hello-world와 animation 두 개만:**

```sh
make -C dali-ui/build/web/samples \
  DALI_WEB_SAMPLE_DIRS='hello-world;animation' \
  DALI_WEB_SAMPLE_TARGETS='hello-world.example;animation.example'
```

`DALI_WEB_SAMPLE_DIRS`는 `dali-ui/samples/` 아래 폴더 이름,
`DALI_WEB_SAMPLE_TARGETS`는 그 폴더의 `CMakeLists.txt`에 선언된 타깃 이름입니다.
여러 개를 지정할 때는 위와 같이 `;`로 구분하고 따옴표로 묶습니다.
한 폴더에 앱이 여럿 있으므로 원하는 타깃까지 함께 지정하는 편이 간단합니다.

**기본 샘플 11개를 다시 빌드하려면:**

```sh
make -C dali-ui/build/web samples
```

## 4. 빌드한 샘플 실행하기

**에디터에서:**

```sh
make -C dali-ui/build/web editor
```

**http://127.0.0.1:8787/** 의 상단 Application에서 샘플을 선택합니다.
빌드 결과가 있으면 바로 실행합니다. 소스 수정 후 **Build & Run**을 누르면
저장 → 빌드 → 프리뷰 실행까지 수행합니다.
외부 터미널에서 다시 빌드했다면 에디터 페이지를 새로고침해 결과를 확인합니다.

Scene의 **App default**는 `text-gradient`에 920 × 880, 다른 앱에 800 × 600을
사용합니다. 프리뷰 패널 크기를 조절해도 논리 Scene 크기는 유지합니다.
다른 고정 크기가 필요한 앱은 Scene에서 Desktop / Portrait / TV / custom을 선택합니다.

**에디터 없이 샘플만 브라우저에서:**

```sh
make -C dali-ui/build/web serve
```

브라우저에서 다음 주소를 엽니다. 서버 종료는 `Ctrl+C`입니다.

- 기본 앱: **http://127.0.0.1:8765/text-gradient.example.html**
- hello-world: **http://127.0.0.1:8765/hello-world.example.html**
- 마지막으로 선택해 빌드한 샘플 목록: **http://127.0.0.1:8765/**

제공한 서버가 WASM threads 실행에 필요한 HTTP 헤더를 설정합니다.
HTML 파일을 직접 열거나 일반 정적 서버로 대체하면 실행 조건이 달라집니다.

## 5. 결과물과 로그

| 항목 | 기본 위치 |
| --- | --- |
| Web 라이브러리·헤더 | `/tmp/dali-web-prefix` |
| 빌드 디렉터리 | `/tmp/dali-web-build/{core,adaptor,ui,samples}` |
| 배포할 HTML / JS / WASM / 데이터 | `/tmp/dali-web-build/samples/dist/` |
| 컴포넌트별 빌드 로그 | `/tmp/dali-web-build/logs/{core,adaptor,ui,samples}.log` |
| SDK·외부 의존성 캐시 | `$HOME/.cache/dali-web` |

예를 들어 샘플 빌드 로그를 보려면:

```sh
tail -n 80 /tmp/dali-web-build/logs/samples.log
```

빌드 중 전체 출력을 보려면:

```sh
DALI_WEB_VERBOSE=1 make -C dali-ui/build/web samples
```

`/tmp`가 정리되면 `world`로 다시 빌드합니다.
결과물을 작업 폴더에 계속 보관하려면 **빌드·서버 실행 전 같은 터미널에서**
다음 설정을 한 번 적용하고 위 명령을 실행합니다.

```sh
export DALI_WEB_PREFIX="$PWD/out/web-prefix"
export DALI_WEB_BUILD="$PWD/out/web-build"
```

지원 범위와 세부 설정은
[Web backend 안내](../../../dali-adaptor/build/web/README.md),
[앱 에디터 안내](../../tools/web-editor/README.md)를 참고하십시오.
CanvasRenderer·Lottie 등 아직 Web 구현이 없는 기능을 쓰는 샘플은
빌드가 성공해도 해당 부분이 표시되지 않을 수 있습니다.
