# EVMC: Ethereum Client-VM Connector API.
# Copyright 2020 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(tool-utils STATIC utils.cpp utils.hpp)
add_library(evmc::tool-utils ALIAS tool-utils)
target_compile_features(tool-utils PUBLIC cxx_std_17)
target_include_directories(tool-utils PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(tool-utils PUBLIC evmc::evmc)
