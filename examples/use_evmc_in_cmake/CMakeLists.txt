# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

# This example shows how to use evmc INTERFACE library from evmc CMake package.

cmake_minimum_required(VERSION 3.5)

project(use_evmc_in_cmake)

find_package(evmc CONFIG REQUIRED)

add_executable(use_evmc_in_cmake use_evmc_in_cmake.c)
target_link_libraries(use_evmc_in_cmake PRIVATE evmc::evmc)



# Only for integration tests.
if(NOT COMMAND evmc_add_vm_test)
    message(FATAL_ERROR "Function evmc_add_vm_test() not in EVMC.cmake module")
endif()
