# QueryInterface sample

core의 Animation, UI의 Label, 앱이 직접 만든 SampleCard를 같은
`QueryInterface<I>(BaseHandle)`로 사용하는 예제다. Factory·TypeRegistry 등록은
추가하지 않는다. 앱 인터페이스 UUID와 Impl 매핑도 설치된 core 생성기가 만든다.

## 패치 구성

기반 기능은 core1 + adaptor1 + ui1만으로 빌드·사용할 수 있다.
이 샘플은 core2의 IPlayback과 ui2의 ITextContent/Label 적용까지 필요하다.
core 공통 UTC는 테스트 전용 인터페이스를 생성하므로 두 플랫폼 인터페이스에 의존하지 않는다.

## 빌드·실행

현재 PoC는 GCC/Clang + RTTI, shared DALi SDK를 대상으로 한다.
Python 3.9 이상과 버전이 일치하는 Python clang bindings/libclang 18 이상이 필요하다.
Ubuntu에서는 core의 `build/scripts/setup-query-interface-tools.sh`로 준비한
Python 경로를 `DALI_QUERY_INTERFACE_PYTHON`으로 지정할 수 있다.
SDK 빌드 순서는 core → adaptor → ui다.
GBS에서는 같은 CMake 생성 경로를 사용하며 `python-clang >= 18`,
`clang-devel >= 18`과 갱신된 DALi devel/integration-devel RPM이 필요하다.
core devel RPM이 공용 생성기와 CMake 함수를 제공한다.

```sh
export DESKTOP_PREFIX=/tmp/dali-query-interface-prefix
export PKG_CONFIG_PATH="$DESKTOP_PREFIX/lib/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig"
export LD_LIBRARY_PATH="$DESKTOP_PREFIX/lib"

cmake -S dali-ui/samples/query-interface -B /tmp/dali-qi-sample -DCMAKE_BUILD_TYPE=Release
cmake --build /tmp/dali-qi-sample -j8
DALI_WINDOW_WIDTH=680 DALI_WINDOW_HEIGHT=420 dali-ui/samples/query-interface/bin/query-interface.example
```

위 명령은 workspace root 기준이다. 생성기는 configure 단계에서 Python·바인딩·
libclang과 최소 AST 분석을 점검한다. 화면 실행에는 X11 등의
사용 가능한 DALi window backend가 필요하다.

| 키 | 동작 |
|---|---|
| Space | `IPlayback::Play()`로 카드 투명도 애니메이션 재생·재개 |
| P | `IPlayback::Pause()` |
| S | `IPlayback::Stop()` |
| T | 앱 component의 `ITextContent::SetText()` 호출 |
| R | 앱의 `IResettable::ResetContent()` 호출 |
| Esc | 종료 |

상단 상태 Label도 ITextContent로 변경한다. SampleCardImpl은 공개 ViewImpl을
상속하지만, ViewImpl 자체에는 인터페이스를 추가하지 않았다.

## 인터페이스 추가

`interfaces/`에 `DALI_QUERY_INTERFACE`로 표시한 인터페이스를 선언하고,
앱의 Impl에서 상속·구현한다. 새로운 구현 헤더라면
`query-interface-headers.list`에 샘플 루트 기준 경로를 한 줄 추가한다.
이미 등록된 헤더에 인터페이스를 추가할 때는 목록 변경도 필요 없다.
CMake 수정, UUID 지정, 인터페이스별 구현 매핑 등록은 필요 없다.
파일 목록은 분석 범위만 정하며, 상속 관계와 포인터 변환은 Clang AST로 분석한다.

## 자동 검사

```sh
dali-ui/samples/query-interface/bin/query-interface.example --self-test
ctest --test-dir /tmp/dali-qi-sample --output-on-failure
```

`--self-test`는 별도 객체로 **기능 검사 39개 + 실제 프레임 검사 4개**를 실행한 뒤
종료한다. 오류는 `SELF-TEST FAILED`와 non-zero exit code로 보고한다.

- Play/Pause/Stop, SetText/GetText, ResetContent의 실제 결과와 앱 함수 호출 횟수.
- UTF-8·빈 문자열, 미지원/빈 Handle, const 조회, 반복 조회 identity.
- 한 Impl의 서로 다른 인터페이스 주소와 동일 소유 객체, 양방향 `As`.
- 기반·파생 Impl에 생성된 provider가 같은 인터페이스 subobject를 반환하는지.
- 복사/이동/대입/자기 대입, 정확한 참조 증가·해제, 마지막 참조에서 한 번의 파괴.
- 실제 render/update 프레임에서 재생의 진행, 일시정지 유지, 재개, 정지 유지.

생성 결과와 실제 실행의 UUID도 비교할 수 있다. 아래 core/UI 경로는 이 작업에서
사용한 out-of-tree build 경로다. 다른 build 경로를 쓰면 해당 디렉터리로 바꾼다.

```sh
dali-ui/samples/query-interface/bin/query-interface.example --self-test > /tmp/dali-qi-runtime.log 2>&1
python3 dali-ui/samples/query-interface/validation/check-generated.py \
  --core /tmp/dali-qi-core/query-interfaces/dali3-core \
  --ui /tmp/dali-qi-ui/dali-ui-foundation/query-interfaces/dali3-ui-foundation \
  --app /tmp/dali-qi-sample/query-interfaces/query-interface.example \
  --sdk "$DESKTOP_PREFIX" --runtime-log /tmp/dali-qi-runtime.log
"$DALI_QUERY_INTERFACE_PYTHON" dali-core/automated-tests/scripts/test-query-interface-generator.py
```

`check-generated.py`는 manifest↔생성 헤더↔설치 헤더↔실행 시 UUID 일치,
정확한 provider 6개, 의존 모듈 UUID 재사용, 일반 인터페이스의 컴파일 거부를 확인한다.
생성기 테스트는 UUID 결정성, 의존 모듈 ID 일치, 매핑과 다이아몬드 등을 검사한다.
모듈 빌드는 ABI 기준선을 비교하거나 갱신하지 않는다. 생성 manifest는 현재 선언의
계약과 UUID이며, 배포 ABI 관리는 DALi 전체의 별도 시스템에서 처리한다.

## 기대 결과

- Native self-test: 함수·수명 검사 39개와 실제 프레임 검사 4개 통과.
- CTest: `query-interface-self-test` 성공.
- UUID 검사: manifest, 생성/설치 헤더, 실행 결과의 ID 3개 일치.
- 매핑 검사: core/UI/app의 provider 6개와 의존 인터페이스 ID 재사용 확인.

실제 검증 환경과 결과는 workspace의 `DALI_QUERY_INTERFACE_VALIDATION.md`에 기록한다.
`validation/legacy-client.cpp`는 변경 전 SDK로 컴파일한 바이너리를 새 SDK에서
실행할 때 쓰는 호환성 probe다. 새 SDK로만 빌드하면 이전 바이너리 호환성 검사가 아니다.
