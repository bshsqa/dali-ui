# DALi Web PoC 자료와 재현

이 디렉터리는 세 DALi 저장소만 받아도 Web PoC의 안내와 기획 자료를 함께
확인할 수 있도록 정리한 문서 모음입니다.

- [빌드·실행 가이드](WEB_BUILD_GUIDE.md)
- [Web 기반 DALi UI 개발·저작 공통 기반 구축 계획서](DALi_Web_Platform_Proposal/README.md)
- [앱 에디터 안내](../../tools/web-editor/README.md)
- [Web backend 구현·지원 범위](../../../dali-adaptor/build/web/README.md)

## 재현에 필요한 저장소

아래 세 저장소를 같은 부모 디렉터리에 둡니다.

```text
DALi/
  dali-core/
  dali-adaptor/
  dali-ui/
```

`dali-core`와 `dali-adaptor`의 Web 변경은 Gerrit 리뷰로 배포되므로,
리뷰가 병합되기 전에는 각 리뷰의 최신 patch set을 받아야 합니다.
기본 `devel/master`만 clone해서는 Web 전용 코드가 포함되지 않을 수 있습니다.
두 Web 변경이 의존하는 고정 논리 Scene 변경도 해당 커밋의 이력에 포함되어야 합니다.
`dali-ui`는 `origin`의 `daliWeb` 브랜치를 사용합니다.

### 이번 PoC 버전 받기

세 저장소를 새로 clone한 상태에서 다음 명령으로 작업 브랜치를 만듭니다.
각 저장소의 `origin`은 기존 Tizen Gerrit(core·adaptor)과
`https://github.com/bshsqa/dali-ui.git`(ui)을 가리켜야 합니다.

```sh
git -C dali-core fetch origin refs/changes/61/359361/1
git -C dali-core switch -c daliWeb FETCH_HEAD
git -C dali-adaptor fetch origin refs/changes/62/359362/1
git -C dali-adaptor switch -c daliWeb FETCH_HEAD
git -C dali-ui fetch origin daliWeb
git -C dali-ui switch -c daliWeb --track origin/daliWeb
```

이미 `daliWeb` 브랜치로 clone한 저장소는 해당 `switch -c` 명령을 생략합니다.
core·adaptor의 위 리뷰 커밋에는 고정 Scene 선행 변경까지 포함되어 있으므로
별도 cherry-pick은 필요하지 않습니다.

- [core Web 리뷰 359361](https://review.tizen.org/gerrit/c/platform/core/uifw/dali-core/+/359361)
- [adaptor Web 리뷰 359362](https://review.tizen.org/gerrit/c/platform/core/uifw/dali-adaptor/+/359362)
- [ui daliWeb 브랜치](https://github.com/bshsqa/dali-ui/tree/daliWeb)

### 빌드·실행

Ubuntu의 시스템 빌드 도구를 준비한 뒤 `DALi/`에서 실행합니다.
SDK·외부 라이브러리·폰트는 첫 빌드가 내려받습니다.

```sh
make -C dali-adaptor/build/web world
make -C dali-ui/build/web editor
```

브라우저에서 **http://127.0.0.1:8787/** 를 엽니다.
세 저장소 밖의 별도 workspace 스크립트나 사전 생성된 WASM·npm 디렉터리는
필요하지 않습니다. 세부 요구 버전과 설치 명령은 위 빌드 가이드를 따릅니다.

2026-10-28에 커밋된 소스만 별도 작업 폴더에 체크아웃하고 빈 빌드·설치 경로에서
전체 빌드를 검증했습니다. SDK·다운로드 캐시는 재사용했으며, 외부 라이브러리는
새 prefix에 다시 빌드했습니다. 기본 샘플 브라우저 테스트 10개, 에디터 브라우저
테스트 9개, 백엔드 테스트 3개가 통과했습니다.

이 자료는 로컬 WebGL 2 PoC를 위한 것입니다. 계획서의 서버 서비스·시각 저작·
WebGPU 기능은 향후 구축 대상으로 구분합니다.
