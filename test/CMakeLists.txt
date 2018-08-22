# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(integration)
add_subdirectory(unittests)
add_subdirectory(vmtester)
