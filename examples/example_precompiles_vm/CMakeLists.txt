# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(evmc-example-precompiles-vm SHARED example_precompiles_vm.cpp)
target_link_libraries(evmc-example-precompiles-vm PRIVATE evmc)
set_source_files_properties(example_precompiles_vm.cpp PROPERTIES
    COMPILE_DEFINITIONS PROJECT_VERSION="${PROJECT_VERSION}")