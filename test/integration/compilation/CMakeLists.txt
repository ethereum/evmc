# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

add_library(test-compile-c99 STATIC compilation_test.c)
target_link_libraries(test-compile-c99 PRIVATE evmc)
set_target_properties(test-compile-c99 PROPERTIES C_STANDARD 99 C_EXTENSIONS OFF)

add_library(test-compile-c11 STATIC ${PROJECT_SOURCE_DIR}/include/evmc/evmc.h compilation_test.c)
target_link_libraries(test-compile-c11 PRIVATE evmc)
set_target_properties(test-compile-c11 PROPERTIES C_STANDARD 11 C_EXTENSIONS OFF)

add_library(test-compile-cxx98 STATIC compilation_test.cpp)
target_link_libraries(test-compile-cxx98 PRIVATE evmc)
set_target_properties(test-compile-cxx98 PROPERTIES CXX_STANDARD 98 CXX_EXTENSIONS OFF)

add_library(test-compile-cxx11 STATIC compilation_test.cpp)
target_link_libraries(test-compile-cxx11 PRIVATE evmc)
set_target_properties(test-compile-cxx11 PROPERTIES CXX_STANDARD 11 CXX_EXTENSIONS OFF)
