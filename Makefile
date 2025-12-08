# This requires emsdk

comma := ,
empty :=
space := $(empty) $(empty) # This trick creates a variable containing a single space

CC := emcc

SRC_DIR := src
BUILD_DIR := build

OUT_NAME := paxl

CC_DEBUG_FLAGS := -O0 
CC_FLAGS := -O3 -flto

EXPORTS := paxml

EXPORTED := $(subst $(space),$(comma),$(foreach export,$(EXPORTS),'_$(export)'))

DEBUG=n

ifeq (y,$(DEBUG))
	CC_FLAGS = $(CC_DEBUG_FLAGS)
endif

SRC := $(shell find $(SRC_DIR) -name '*.c')

.PHONY: all clean

all: $(BUILD_DIR)/$(OUT_NAME).js

$(BUILD_DIR)/$(OUT_NAME).js:
	@mkdir -p $(BUILD_DIR)
	$(CC) $(SRC) $(CC_FLAGS) \
		-s MODULARIZE=1 \
		-s EXPORT_NAME=$(OUT_NAME) \
		-s EXPORTED_FUNCTIONS="[$(EXPORTED)]" \
		-s EXPORTED_RUNTIME_METHODS=['stringToNewUTF8','UTF8ToString'] \
		-s EXPORT_ES6=1	\
		 -o $@

clean:
	-rm -rf $(BUILD_DIR)
