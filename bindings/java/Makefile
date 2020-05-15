# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019-2020 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

JAVA_HOME:=$(shell java -XshowSettings:properties -version 2>&1 > /dev/null | grep 'java.home' | sed 's/\s*java.home = //' | sed 's/\/jre//')

SOURCE_DIR = $(realpath $(CURDIR)/../..)
OUT_DIR = $(CURDIR)/c/build
BUILD_DIR = $(OUT_DIR)/_cmake_build

build: gradlew $(OUT_DIR)/lib/libevmc-java.so
	mkdir -p ./java/build
	./gradlew --no-daemon clean spotlessApply build

test: build
	./gradlew --no-daemon test

gradlew:
	gradle --no-daemon setup

$(OUT_DIR)/lib/libevmc-java.so:
	mkdir -p $(BUILD_DIR)
	(cd $(BUILD_DIR) && cmake $(SOURCE_DIR) -DCMAKE_INSTALL_PREFIX=$(OUT_DIR) -DEVMC_JAVA=ON -DJAVA_HOME=$(JAVA_HOME) -DEVMC_EXAMPLES=ON)
	cmake --build $(OUT_DIR)/_cmake_build --target install

clean:
	rm -rf $(OUT_DIR)
	rm -rf build
	rm -rf ./java/build/
