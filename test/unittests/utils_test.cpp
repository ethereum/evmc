// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/utils.hpp>
#include <gtest/gtest.h>

TEST(utils, hex)
{
    auto data = evmc::bytes{0x0, 0x1, 0xa, 0xf, 0x1f, 0xa0, 0xff, 0xf0};
    EXPECT_EQ(evmc::hex(data), "00010a0f1fa0fff0");
}

TEST(utils, from_hex_empty)
{
    EXPECT_TRUE(evmc::from_hex({}).empty());
}

TEST(utils, from_hex_odd_input_length)
{
    EXPECT_THROW(evmc::from_hex("0"), std::length_error);
}

TEST(utils, from_hex_capital_letters)
{
    EXPECT_EQ(evmc::from_hex("ABCDEF"), (evmc::bytes{0xab, 0xcd, 0xef}));
}

TEST(utils, from_hex_invalid_encoding)
{
    EXPECT_THROW(evmc::from_hex({"\0\0", 2}), std::out_of_range);
}

TEST(utils, hex_byte)
{
    auto b = uint8_t{};
    EXPECT_EQ(evmc::hex(b), "00");
    b = 0xaa;
    EXPECT_EQ(evmc::hex(b), "aa");
}
