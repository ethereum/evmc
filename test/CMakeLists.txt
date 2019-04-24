# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_C_EXTENSIONS OFF)
endif()

if(EVMC_TESTING)
    add_subdirectory(integration)
    add_subdirectory(unittests)
endif()

add_subdirectory(vmtester)
