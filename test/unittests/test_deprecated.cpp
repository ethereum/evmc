// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/helpers.hpp>

#include <gtest/gtest.h>
#include <map>
#include <unordered_map>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

TEST(helpers, fnv1a)
{
    const uint8_t text[] = {'E', 'V', 'M', 'C'};
    const auto h = fnv1a(text, sizeof(text));
    EXPECT_EQ(h, sizeof(size_t) == 8 ? 0x15e05d6d22fed89a : 0xffaa6a9a);
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
