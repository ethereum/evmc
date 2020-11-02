# EVMC: Ethereum Client-VM Connector API.
# Copyright 2020 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(tool-commands commands.hpp run.cpp)
add_library(evmc::tool-commands ALIAS tool-commands)
target_compile_features(tool-commands PUBLIC cxx_std_14)
target_include_directories(tool-commands PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(tool-commands PRIVATE evmc::tool-utils evmc::mocked_host)
