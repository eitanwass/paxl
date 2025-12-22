# This requires emsdk

comma := ,
empty := 
space := $(empty) $(empty)

CC := emcc

SRC_DIR := src
LIBS_DIR := libs
BUILD_DIR := build

OUT_NAME := paxl_core

DEBUG ?= n
W_ENTRY ?= n
STANDALONE ?= y

EM_FLAGS :=

CC_FLAGS := -flto -mnontrapping-fptoint
# CC_FLAGS += -Ilibs/parson
CC_FLAGS += -Ilibs/yyjson

CC_FLAGS += -DYYJSON_DISABLE_READER=1
CC_FLAGS += -DYYJSON_DISABLE_INCR_READER=1

EXPORTS :=
RUNTIME_EXPORTS :=

# EXPORTS += parse free_json
# -sEXPORTED_FUNCTIONS=$(FMT_EXPORTS) \

RUNTIME_EXPORTS += stringToNewUTF8 UTF8ToString

FMT_EXPORTS := "[$(subst $(space),$(comma),$(foreach export,$(EXPORTS),'_$(export)'))]"
FMT_RUNTIME_EXPORTS := "[$(subst $(space),$(comma),$(foreach export,$(RUNTIME_EXPORTS),'$(export)'))]"

ifeq ($(DEBUG),y)
	CC_FLAGS += -O0
else
	CC_FLAGS += -O3 -ffast-math
endif

ifneq ($(W_ENTRY),y)
	CC_FLAGS += --no-entry
endif

ifeq ($(STANDALONE),y)
	EM_FLAGS += -sSTANDALONE_WASM
	CC_FLAGS += -DSTANDALONE
endif

# SRCS := $(shell find $(SRC_DIR) -name '*.c')
SRCS :=
SRCS += $(SRC_DIR)/core/xml.c
SRCS += $(SRC_DIR)/core/parser.c
# SRCS += $(LIBS_DIR)/parson/parson.c
SRCS += $(LIBS_DIR)/yyjson/yyjson.c

.PHONY: all format clean

all: $(BUILD_DIR)/$(OUT_NAME)

$(BUILD_DIR)/$(OUT_NAME):
	@mkdir -p $(BUILD_DIR)
	$(CC) $(SRCS) $(CC_FLAGS) $(EM_FLAGS) \
	-sEXPORTED_RUNTIME_METHODS=$(FMT_RUNTIME_EXPORTS) \
	-sEXPORT_ES6 \
	-sMODULARIZE \
	-sEXPORT_NAME=$(OUT_NAME) \
	-o $@.js

	@if [ "$(STANDALONE)" = "y" ]; then \
		cp ./src/standalone_paxl.js $(BUILD_DIR)/paxl.js; \
	else \
		cp ./src/paxl.js $(BUILD_DIR)/paxl.js; \
	fi

format:
	-clang-format -i --style=file ./src/**/*

clean:
	-rm -rf $(BUILD_DIR)
