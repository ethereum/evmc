# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(loader-mocked STATIC ${PROJECT_SOURCE_DIR}/lib/loader/loader.c)
target_link_libraries(loader-mocked PRIVATE evmc)
target_compile_definitions(loader-mocked PRIVATE EVMC_LOADER_MOCK=1)

add_library(vm-mock SHARED vm_mock.c)
target_link_libraries(vm-mock PRIVATE evmc)

add_library(vm-mock-default SHARED vm_mock_default.c)
target_link_libraries(vm-mock-default PRIVATE evmc)


if(UNIX)
    set(cmd create_symlink)
else()
    set(cmd copy)
endif()

add_custom_command(
    TARGET vm-mock POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> libaaa.so
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> double_prefix_aaa.evm
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> double-prefix-aaa.evm
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> eee-bbb.dll
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> libeee1.so
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> eee2.so
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> libeee3.x
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> eee4
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> _
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> lib_.so
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> ../aaa.evm
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> failure.vm
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> abi42.vm

    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock-default> default.evmc

    COMMAND ${CMAKE_COMMAND} -E touch empty.file
)

add_executable(
    evmc-test
    loader_mock.h
    test_cpp.cpp
    test_helpers.cpp
    test_instructions.cpp
    test_loader.cpp
)

target_link_libraries(evmc-test PRIVATE evmc-example-vm instructions loader-mocked GTest::gtest GTest::main)
set_target_properties(evmc-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
add_dependencies(evmc-test vm-mock)
