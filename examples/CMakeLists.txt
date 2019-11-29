# EVMC: Ethereum Client-VM Connector API.
# Copyright 2016-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(GNUInstallDirs)

add_subdirectory(example_vm)
add_subdirectory(example_precompiles_vm)

add_library(evmc-example-host STATIC example_host.cpp)
target_link_libraries(evmc-example-host PRIVATE evmc::evmc_cpp)

add_executable(evmc-example-static example.c)
target_link_libraries(evmc-example-static PRIVATE evmc-example-host evmc::example-vm-static evmc::evmc)
target_compile_definitions(evmc-example-static PRIVATE STATICALLY_LINKED_EXAMPLE)

add_executable(evmc-example example.c)
target_link_libraries(evmc-example PRIVATE evmc-example-host evmc::loader)
