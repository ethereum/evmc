# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(GoogleTest)

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

add_library(loader-mocked STATIC ${PROJECT_SOURCE_DIR}/lib/loader/loader.c)
target_link_libraries(loader-mocked PRIVATE evmc::evmc)
target_compile_definitions(loader-mocked PRIVATE EVMC_LOADER_MOCK=1)

add_executable(
    evmc-unittests
    cpp_test.cpp
    example_vm_test.cpp
    helpers_test.cpp
    instructions_test.cpp
    loader_mock.h
    loader_test.cpp
    mocked_host_test.cpp
    filter_iterator_test.cpp
    tooling_test.cpp
    hex_test.cpp
)

target_link_libraries(
    evmc-unittests
    PRIVATE
    loader-mocked
    evmc::example-vm-static
    evmc::example-precompiles-vm-static
    evmc::instructions
    evmc::evmc_cpp
    evmc::tooling
    GTest::gtest_main
)
target_include_directories(evmc-unittests PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_options(
    evmc-unittests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:-wd4068> # allow unknown pragma
)

gtest_add_tests(TARGET evmc-unittests TEST_PREFIX ${PROJECT_NAME}/unittests/ TEST_LIST unittests)

set_tests_properties(${unittests} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/unittests-%p.profraw)
