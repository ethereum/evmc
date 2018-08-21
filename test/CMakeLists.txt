# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the MIT License. See the LICENSE file.

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(integration)
add_subdirectory(unittests)
add_subdirectory(vmtester)
