# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(evmc INTERFACE)
add_library(evmc::evmc ALIAS evmc)
target_include_directories(evmc INTERFACE $<BUILD_INTERFACE:${include_dir}>$<INSTALL_INTERFACE:include>)

add_subdirectory(instructions)
add_subdirectory(loader)
add_subdirectory(mocked_host)
