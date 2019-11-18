# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(mocked_host INTERFACE)
target_sources(mocked_host INTERFACE $<BUILD_INTERFACE:${include_dir}/evmc/mocked_host.hpp>)

add_library(evmc::mocked_host ALIAS mocked_host)
target_include_directories(mocked_host INTERFACE
    $<BUILD_INTERFACE:${include_dir}>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(EVMC_INSTALL)
    install(TARGETS mocked_host EXPORT evmcTargets)
endif()
