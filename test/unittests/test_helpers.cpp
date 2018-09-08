// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#include <evmc/helpers.hpp>

#include <gtest/gtest.h>

#include <map>
#include <unordered_map>

TEST(helpers, maps)
{
    std::map<evmc_address, bool> addresses;
    addresses[{}] = true;
    ASSERT_EQ(addresses.size(), 1);

    std::unordered_map<evmc_address, bool> unordered_addresses;
    unordered_addresses.emplace(*addresses.begin());
    EXPECT_EQ(unordered_addresses.size(), 1);

    std::map<evmc_bytes32, bool> storage;
    storage[{}] = true;
    ASSERT_EQ(storage.size(), 1);

    std::unordered_map<evmc_bytes32, bool> unordered_storage;
    unordered_storage.emplace(*storage.begin());
    EXPECT_EQ(unordered_storage.size(), 1);
}
