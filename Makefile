# This requires emsdk

comma := ,
empty := 
space := $(empty) $(empty)

CC := emcc

SRC_DIR := src
LIBS_DIR := libs
BUILD_DIR := build

OUT_NAME := paxl_core

CC_DEBUG_FLAGS := -O0 
CC_FLAGS := -O3 -flto
CC_FLAGS += -Ilibs/parson

EXPORTS := parse parse_new free_json
RUNTIME_EXPORTS := stringToNewUTF8
RUNTIME_EXPORTS += UTF8ToString

FMT_EXPORTS := "[$(subst $(space),$(comma),$(foreach export,$(EXPORTS),'_$(export)'))]"
FMT_RUNTIME_EXPORTS := "[$(subst $(space),$(comma),$(foreach export,$(RUNTIME_EXPORTS),'$(export)'))]"

DEBUG=n

ifeq (y,$(DEBUG))
	CC_FLAGS = $(CC_DEBUG_FLAGS)
endif

SRCS := $(shell find $(SRC_DIR) -name '*.c')
SRCS += $(LIBS_DIR)/parson/parson.c

.PHONY: all format clean

all: $(BUILD_DIR)/$(OUT_NAME).js

$(BUILD_DIR)/$(OUT_NAME).js:
	@mkdir -p $(BUILD_DIR)
	$(CC) $(SRCS) $(CC_FLAGS) \
		-s MODULARIZE=1 \
		-s EXPORT_NAME=$(OUT_NAME) \
		-s EXPORTED_FUNCTIONS=$(FMT_EXPORTS) \
		-s EXPORTED_RUNTIME_METHODS=$(FMT_RUNTIME_EXPORTS) \
		-s EXPORT_ES6=1	\
		-s EVAL_CTORS=1 \
		 -o $@
	cp ./src/paxl.js $(BUILD_DIR)/

format:
	-clang-format -i --style=file ./src/**/*

clean:
	-rm -rf $(BUILD_DIR)
