# Web build and samples

WASM 빌드·실행·설정·지원 범위는
[DALi Web backend 안내](../../../dali-adaptor/build/web/README.md)를 참고하십시오.
세 저장소가 동일한 부모 디렉터리에 있어야 합니다.

```sh
# DALi workspace root
make -C dali-adaptor/build/web world
make -C dali-ui/build/web serve
```
