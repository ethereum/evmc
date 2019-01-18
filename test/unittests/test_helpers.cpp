// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

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

TEST(helpers, is_zero)
{
    auto a = evmc_address{};
    EXPECT_TRUE(is_zero(a));
    a.bytes[0] = 1;
    EXPECT_FALSE(is_zero(a));

    auto b = evmc_bytes32{};
    EXPECT_TRUE(is_zero(b));
    b.bytes[0] = 1;
    EXPECT_FALSE(is_zero(b));
}
