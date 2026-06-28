include Makefile.common
include Makefile.test

.PHONY: all build docs benchmark format clean
.DEFAULT_GOAL := all

all: build

build: $(DIST_DIR)/$(OUT_NAME)
	npx tsc
	npm pack

# The GitHub Pages demo (docs/) is served as static files with no build step of
# its own, so the real browser build has to be copied in after `make build`
# rather than hand-duplicated - otherwise the demo's wasm-loading code silently
# drifts from the published npm package's browser entry point.
docs: build
	cp $(DIST_DIR)/paxl-browser.js docs/paxl-browser.js
	cp $(DIST_DIR)/paxl-browser.js.map docs/paxl-browser.js.map
	cp $(DIST_DIR)/paxl_core.wasm docs/paxl_core.wasm

benchmark:
	npm run test -C ./benchmarks

format:
	@clang-format -i --style=file ./src/**/*.c

clean:
	@rm -rf $(DIST_DIR)
	@find $(TEST_DIR) -type f -name 'test_*' ! -name '*.*' -delete
