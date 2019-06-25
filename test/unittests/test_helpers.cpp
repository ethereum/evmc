// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/helpers.h>
#include <evmc/helpers.hpp>

#include <gtest/gtest.h>

#include <map>
#include <unordered_map>


// Compile time checks:

static_assert(sizeof(evmc_bytes32) == 32, "evmc_bytes32 is too big");
static_assert(sizeof(evmc_address) == 20, "evmc_address is too big");
static_assert(sizeof(evmc_result) <= 64, "evmc_result does not fit cache line");
static_assert(sizeof(evmc_instance) <= 64, "evmc_instance does not fit cache line");
static_assert(offsetof(evmc_message, value) % 8 == 0, "evmc_message.value not aligned");

// Check enums match int size.
// On GCC/clang the underlying type should be unsigned int, on MSVC int
static_assert(sizeof(evmc_call_kind) == sizeof(int),
              "Enum `evmc_call_kind` is not the size of int");
static_assert(sizeof(evmc_revision) == sizeof(int), "Enum `evmc_revision` is not the size of int");

static constexpr size_t optionalDataSize =
    sizeof(evmc_result) - offsetof(evmc_result, create_address);
static_assert(optionalDataSize == sizeof(evmc_result_optional_storage), "");


TEST(helpers, fnv1a)
{
    const uint8_t text[] = {'E', 'V', 'M', 'C'};
    const auto h = fnv1a(text, sizeof(text));
    EXPECT_EQ(h, sizeof(size_t) == 8 ? 0x15e05d6d22fed89a : 0xffaa6a9a);
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

TEST(helpers, release_result)
{
    auto r1 = evmc_result{};
    evmc_release_result(&r1);

    static evmc_result r2;
    static bool e;

    e = false;
    r2 = evmc_result{};
    r2.release = [](const evmc_result* r) { e = r == &r2; };
    EXPECT_FALSE(e);
    evmc_release_result(&r2);
    EXPECT_TRUE(e);
}
