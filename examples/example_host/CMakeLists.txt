# EVMC: Ethereum Client-VM Connector API
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

add_library(evmc-example-host STATIC example_host.cpp)
target_link_libraries(evmc-example-host PRIVATE evmc::evmc)
set_target_properties(evmc-example-host PROPERTIES
    DEBUG_POSTFIX ""
    RUNTIME_OUTPUT_DIRECTORY ..
    LIBRARY_OUTPUT_DIRECTORY ..
)
