OS:=$(shell uname -s | tr '[:upper:]' '[:lower:]')
ifeq ($(OS), linux)
	EXT:=so
	OS_LFLAGS:=
	JAVA_HOME:=$(shell java -XshowSettings:properties -version 2>&1 > /dev/null | grep 'java.home' | sed 's/\s*java.home = //' | sed 's/\/jre//')
else ifeq ($(OS), darwin)
	EXT:=so
	OS_LFLAGS:=-mmacosx-version-min=$(shell defaults read loginwindow SystemVersionStampAsString) -framework CoreFoundation -framework Security
	JAVA_HOME:=$(shell java -XshowSettings:properties -version 2>&1 > /dev/null | grep 'java.home' | sed 's/\s*java.home = //' | sed 's/\/jre//')
endif

INCLUDES = -I../../include
JAVA_INCLUDES = -I$(JAVA_HOME)/include/$(OS) -I$(JAVA_HOME)/include
JAVA_LIBS = -L$(JAVA_HOME)/lib/server -ljvm
CFLAGS = -O2 -fPIC
LFLAGS = -shared
OUT_DIR = ./c/build

gradlew:
	gradle --no-daemon setup

build: gradlew
	mkdir -p $(OUT_DIR)
	javac ./java/src/main/java/org/ethereum/evmc/EvmcVm.java -h ./c --class-path ./java/src/main/java -s ./java/build
	mv c/org_ethereum_evmc_EvmcVm.h c/evmc-vm.h
	gcc $(DEBUG_FLAG) $(CFLAGS) -c $(INCLUDES) -o $(OUT_DIR)/loader.o ../../lib/loader/loader.c
	gcc $(DEBUG_FLAG) $(CFLAGS) -c $(INCLUDES) $(JAVA_INCLUDES) -o $(OUT_DIR)/host.o ./c/host.c
	gcc $(DEBUG_FLAG) $(CFLAGS) ./c/evmc-vm.c $(INCLUDES) $(JAVA_INCLUDES) $(JAVA_LIBS) $(CFLAGS) $(LFLAGS) -o $(OUT_DIR)/evmc.$(EXT) $(OUT_DIR)/host.o $(OUT_DIR)/loader.o
	gcc $(DEBUG_FLAG) -shared ../../examples/example_vm/example_vm.c $(INCLUDES) -o $(OUT_DIR)/example_vm.$(EXT)
	mkdir -p ./java/build
	./gradlew --no-daemon clean spotlessApply build

test: build
	./gradlew --no-daemon test

format:
	clang-format -i c/evmc-vm.c c/host.c c/host.h

clean:
	rm -rf build
	rm -rf ./java/build/
	rm -rf ./c/build/
