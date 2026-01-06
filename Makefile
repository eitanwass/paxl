include Makefile.common
include Makefile.test

.PHONY: all format clean

all: build

build: $(DIST_DIR)/$(OUT_NAME)

benchmark:
	npm run test -C ./benchmarks

format:
	@clang-format -i --style=file ./src/**/*.c

clean:
	@rm -rf $(DIST_DIR)
	@find $(TEST_DIR) -type f -name 'test_*' ! -name '*.*' -delete
