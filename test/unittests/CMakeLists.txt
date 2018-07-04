# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

add_library(vm-mock SHARED vm_mock.c)

if(UNIX)
    set(cmd create_symlink)
else()
    set(cmd copy)
endif()

add_custom_command(
    TARGET vm-mock POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> ${CMAKE_SHARED_LIBRARY_PREFIX}aaa${CMAKE_SHARED_LIBRARY_SUFFIX}
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> double_prefix_aaa.evm
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> double-prefix-aaa.evm
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> eee-bbb.dll
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> ${CMAKE_SHARED_LIBRARY_PREFIX}eee1${CMAKE_SHARED_LIBRARY_SUFFIX}
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> eee2${CMAKE_SHARED_LIBRARY_SUFFIX}
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> ${CMAKE_SHARED_LIBRARY_PREFIX}eee3
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE_NAME:vm-mock> eee4
    COMMAND ${CMAKE_COMMAND} -E ${cmd} $<TARGET_FILE:vm-mock> ../aaa.evm
    COMMAND ${CMAKE_COMMAND} -E touch empty.file
)

add_executable(
    evmc-test
    test_instructions.cpp
    test_loader.cpp
)

target_link_libraries(evmc-test PRIVATE instructions loader GTest::gtest GTest::main)
set_target_properties(evmc-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
add_dependencies(evmc-test vm-mock)
