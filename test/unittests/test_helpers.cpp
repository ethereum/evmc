// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/helpers.h>

#include <gtest/gtest.h>

// Compile time checks:

static_assert(sizeof(evmc_bytes32) == 32, "evmc_bytes32 is too big");
static_assert(sizeof(evmc_address) == 20, "evmc_address is too big");
static_assert(sizeof(evmc_result) <= 128, "evmc_result does not fit 2 cache lines");
static_assert(sizeof(evmc_vm) <= 64, "evmc_vm does not fit cache line");
static_assert(offsetof(evmc_message, value) % sizeof(size_t) == 0,
              "evmc_message.value not aligned");

// Check enums match int size.
// On GCC/clang the underlying type should be unsigned int, on MSVC int
static_assert(sizeof(evmc_call_kind) == sizeof(int),
              "Enum `evmc_call_kind` is not the size of int");
static_assert(sizeof(evmc_revision) == sizeof(int), "Enum `evmc_revision` is not the size of int");

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
