# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

# Local Hunter configuration.

set(gtest_cxx_flags "${CMAKE_CXX_FLAGS_INIT} -DGTEST_HAS_TR1_TUPLE=0")

hunter_config(
    GTest
    VERSION 1.8.1
    URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz
    SHA1 152b849610d91a9dfa1401293f43230c2e0c33f8
    CMAKE_ARGS BUILD_GMOCK=OFF gtest_force_shared_crt=ON CMAKE_CXX_FLAGS=${gtest_cxx_flags}
)

hunter_config(
    CLI11
    VERSION 1.9.1
    URL https://github.com/CLIUtils/CLI11/archive/v1.9.1.tar.gz
    SHA1 8c32b19a5301726f1bf0b910a91a05d4291deadc
    CMAKE_ARGS CLI11_BUILD_DOCS=OFF CLI11_BUILD_TESTS=OFF CLI11_BUILD_EXAMPLES=OFF CLI11_SINGLE_FILE=OFF
)
