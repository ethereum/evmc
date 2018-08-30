# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()
set(CMAKE_DEBUG_POSTFIX "")

add_library(evmc-example-host STATIC example_host.cpp)
target_link_libraries(evmc-example-host PRIVATE evmc)

add_subdirectory(examplevm)

add_executable(evmc-example example.c)
target_link_libraries(evmc-example PRIVATE evmc-example-host evmc-examplevm evmc)
