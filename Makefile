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

EM_FLAGS :=
EM_FLAGS += -sFILESYSTEM=0
EM_FLAGS += -sASSERTIONS=0
EM_FLAGS += -sSAFE_HEAP=0
EM_FLAGS += -sSTACK_OVERFLOW_CHECK=0
EM_FLAGS += -sSUPPORT_LONGJMP=0
EM_FLAGS += -sERROR_ON_UNDEFINED_SYMBOLS=0
EM_FLAGS += -sSTANDALONE_WASM


CC_FLAGS := -flto -mnontrapping-fptoint
# CC_FLAGS += -Ilibs/parson
CC_FLAGS += -Ilibs/yyjson

CC_FLAGS += -DYYJSON_DISABLE_READER
CC_FLAGS += -DYYJSON_DISABLE_INCR_READER

EXPORTS := 
RUNTIME_EXPORTS :=

EXPORTS += malloc free
# RUNTIME_EXPORTS += stringToNewUTF8 UTF8ToString

FMT_EXPORTS := "[$(subst $(space),$(comma),$(foreach export,$(EXPORTS),'_$(export)'))]"
FMT_RUNTIME_EXPORTS := "[$(subst $(space),$(comma),$(foreach export,$(RUNTIME_EXPORTS),'$(export)'))]"

ifeq ($(DEBUG),y)
	CC_FLAGS += -O0
else
	CC_FLAGS += -O3 -ffast-math
endif

ifneq ($(W_ENTRY),y)
	EM_FLAGS += -sINVOKE_RUN=0
	CC_FLAGS += --no-entry
	CC_FLAGS += -DNO_ENTRY
endif

SRCS :=
SRCS += $(SRC_DIR)/core/xml.c
SRCS += $(SRC_DIR)/core/parser.c
SRCS += $(LIBS_DIR)/yyjson/yyjson.c

.PHONY: all format clean

all: $(BUILD_DIR)/$(OUT_NAME)

$(BUILD_DIR)/$(OUT_NAME):
	@mkdir -p $(BUILD_DIR)
	$(CC) $(SRCS) \
	$(CC_FLAGS) \
	$(EM_FLAGS) \
	-sEXPORTED_FUNCTIONS=$(FMT_EXPORTS) \
	-sEXPORTED_RUNTIME_METHODS=$(FMT_RUNTIME_EXPORTS) \
	-sEXPORT_ES6 \
	-sMODULARIZE \
	-sEXPORT_NAME=$(OUT_NAME) \
	-o $@.js

	cp ./src/paxl_loader.js $(BUILD_DIR)/paxl.js; \

format:
	-clang-format -i --style=file ./src/**/*.c

clean:
	-rm -rf $(BUILD_DIR)
