# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(loader-mocked STATIC ${PROJECT_SOURCE_DIR}/lib/loader/loader.c)
target_link_libraries(loader-mocked PRIVATE evmc)
target_compile_definitions(loader-mocked PRIVATE EVMC_LOADER_MOCK=1)

add_executable(
    evmc-test
    loader_mock.h
    test_cpp.cpp
    test_helpers.cpp
    test_instructions.cpp
    test_loader.cpp
)

target_link_libraries(evmc-test PRIVATE loader-mocked evmc-example-vm-static instructions GTest::gtest GTest::main)
set_target_properties(evmc-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
