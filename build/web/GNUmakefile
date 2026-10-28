WORKSPACE := $(abspath ../../..)
.PHONY: all install samples serve editor
all install:
	bash "$(WORKSPACE)/dali-adaptor/build/web/build.sh" ui
samples:
	bash "$(WORKSPACE)/dali-adaptor/build/web/build.sh" samples
serve:
	bash "$(WORKSPACE)/dali-adaptor/build/web/build.sh" serve
editor:
	bash "$(WORKSPACE)/dali-ui/tools/web-editor/start.sh"
