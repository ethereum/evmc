# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(GoogleTest)

add_library(loader-mocked STATIC ${PROJECT_SOURCE_DIR}/lib/loader/loader.c)
target_link_libraries(loader-mocked PRIVATE evmc)
target_compile_definitions(loader-mocked PRIVATE EVMC_LOADER_MOCK=1)

add_executable(
    evmc-unittests
    loader_mock.h
    test_cpp.cpp
    test_deprecated.cpp
    test_helpers.cpp
    test_instructions.cpp
    test_loader.cpp
)

target_link_libraries(evmc-unittests PRIVATE loader-mocked evmc-example-vm-static evmc-example-host instructions GTest::gtest GTest::main)
set_target_properties(evmc-unittests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)

gtest_add_tests(TARGET evmc-unittests TEST_PREFIX ${PROJECT_NAME}/unittests/)
