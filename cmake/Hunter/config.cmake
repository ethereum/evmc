# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

# Local Hunter configuration.

hunter_config(
    CLI11
    VERSION 1.9.1
    URL https://github.com/CLIUtils/CLI11/archive/v1.9.1.tar.gz
    SHA1 8c32b19a5301726f1bf0b910a91a05d4291deadc
    CMAKE_ARGS CLI11_BUILD_DOCS=OFF CLI11_BUILD_TESTS=OFF CLI11_BUILD_EXAMPLES=OFF CLI11_SINGLE_FILE=OFF
)
