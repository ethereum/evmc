# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

# Local Hunter configuration.

hunter_config(
    GTest
    VERSION 1.8.1
    URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz
    SHA1 152b849610d91a9dfa1401293f43230c2e0c33f8
    CMAKE_ARGS BUILD_GMOCK=OFF gtest_force_shared_crt=ON
)
