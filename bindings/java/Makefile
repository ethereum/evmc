# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019-2020 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

JAVA_HOME:=$(shell java -XshowSettings:properties -version 2>&1 > /dev/null | grep 'java.home' | sed 's/\s*java.home = //' | sed 's/\/jre//')

SOURCE_DIR = $(realpath $(CURDIR)/../..)
OUT_DIR = $(CURDIR)/c/build
BUILD_DIR = $(OUT_DIR)/_cmake_build

all:$(CURDIR)/java/src/test/resources/libevmc.so

build: gradlew
	mkdir -p ./java/build
	./gradlew --no-daemon clean spotlessApply build

test: build
	./gradlew --no-daemon test

gradlew:
	gradle --no-daemon setup

docker-linux: docker/linux.Dockerfile
	docker build -f docker/linux.Dockerfile -t evmc-linux-build $(SOURCE_DIR)

$(CURDIR)/java/src/test/resources/libevmc.so: docker-linux build
	docker run -it -v $(SOURCE_DIR):/workdir --rm evmc-linux-build bash -c "cd /workdir/bindings/java && make compile"
	mkdir -p java/src/test/resources
	cp $(OUT_DIR)/lib/libevmc-java.so $(CURDIR)/java/src/test/resources/libevmc.so
	cp $(OUT_DIR)/lib/libexample-vm.so $(CURDIR)/java/src/test/resources/libexample-vm.so

compile:
	mkdir -p $(BUILD_DIR)
	(cd $(BUILD_DIR) && cmake $(SOURCE_DIR) -DCMAKE_INSTALL_PREFIX=$(OUT_DIR) -DEVMC_JAVA=ON -DJAVA_HOME=$(JAVA_HOME) -DEVMC_EXAMPLES=ON)
	cmake --build $(OUT_DIR)/_cmake_build --target install

clean:
	rm -rf $(OUT_DIR)
	rm -rf build
	rm -rf ./java/build/
