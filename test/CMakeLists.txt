# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_subdirectory(integration)
add_subdirectory(unittests)
add_subdirectory(vmtester)

option(EVMC_VM_FUZZER "Build EVMC VM fuzzer" OFF)
if(EVMC_VM_FUZZER)
    add_library(vm_fuzzer)
endif()
