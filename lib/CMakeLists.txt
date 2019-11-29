# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(evmc INTERFACE)
add_library(evmc::evmc ALIAS evmc)
target_include_directories(evmc INTERFACE $<BUILD_INTERFACE:${include_dir}>$<INSTALL_INTERFACE:include>)

add_library(evmc_cpp INTERFACE)
add_library(evmc::evmc_cpp ALIAS evmc_cpp)
target_include_directories(evmc_cpp INTERFACE $<BUILD_INTERFACE:${include_dir}>$<INSTALL_INTERFACE:include>)
target_link_libraries(evmc_cpp INTERFACE evmc::evmc)

add_subdirectory(instructions)
add_subdirectory(loader)
add_subdirectory(mocked_host)

if(EVMC_INSTALL)
    install(TARGETS evmc evmc_cpp EXPORT evmcTargets)
endif()
