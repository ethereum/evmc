# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

JAVA_HOME:=$(shell java -XshowSettings:properties -version 2>&1 > /dev/null | grep 'java.home' | sed 's/\s*java.home = //' | sed 's/\/jre//')
EXTENSION:="so"
ifeq ($(OS),Windows_NT)
    EXTENSION:=dll
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        EXTENSION:=so
    endif
    ifeq ($(UNAME_S),Darwin)
        EXTENSION:=dylib
    endif
endif

SOURCE_DIR = $(realpath $(CURDIR)/../..)
OUT_DIR = $(CURDIR)/c/build
BUILD_DIR = $(OUT_DIR)/_cmake_build

lint: gradlew
	./gradlew --no-daemon clean spotlessCheck

build: gradlew java/src/test/resources/libexample-vm.$(EXTENSION) java/src/main/resources/libevmc-java.$(EXTENSION)
	mkdir -p ./java/build
	./gradlew --no-daemon clean spotlessApply build

test: build
	./gradlew --no-daemon test

gradlew:
	gradle --no-daemon setup

$(OUT_DIR)/lib/libexample-vm.$(EXTENSION): $(OUT_DIR)/lib/libevmc-java.$(EXTENSION)

$(OUT_DIR)/lib/libevmc-java.$(EXTENSION):
	mkdir -p $(BUILD_DIR)
	(cd $(BUILD_DIR) && cmake $(SOURCE_DIR) -DCMAKE_INSTALL_PREFIX=$(OUT_DIR) -DEVMC_JAVA=ON -DJAVA_HOME=$(JAVA_HOME) -DEVMC_EXAMPLES=ON)
	cmake --build $(OUT_DIR)/_cmake_build --target install

java/src/main/resources/libevmc-java.$(EXTENSION): $(OUT_DIR)/lib/libevmc-java.$(EXTENSION)
	mkdir -p java/src/main/resources
	cp $(OUT_DIR)/lib/libevmc-java.$(EXTENSION) java/src/main/resources/libevmc-java.$(EXTENSION)

java/src/test/resources/libexample-vm.$(EXTENSION): $(OUT_DIR)/lib/libexample-vm.$(EXTENSION)
	mkdir -p java/src/test/resources
	cp $(OUT_DIR)/lib/libexample-vm.$(EXTENSION) java/src/test/resources/libexample-vm.$(EXTENSION)

clean:
	rm -rf $(OUT_DIR)
	rm -rf build
	rm -rf ./java/build/
