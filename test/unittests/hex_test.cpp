// EVMC: Ethereum Client-VM Connector API.
// Copyright 2021 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/hex.hpp>
#include <gtest/gtest.h>
#include <cctype>

using namespace evmc;

TEST(hex, hex_of_byte)
{
    EXPECT_EQ(hex(0x00), "00");
    EXPECT_EQ(hex(0x01), "01");
    EXPECT_EQ(hex(0x55), "55");
    EXPECT_EQ(hex(0x7f), "7f");
    EXPECT_EQ(hex(0x80), "80");
    EXPECT_EQ(hex(0xff), "ff");
}

TEST(hex, hex_of_bytes)
{
    uint8_t data[] = {0x00, 0x01, 0x00, 0xa0, 0xff, 0x00, 0x55, 0x00};
    EXPECT_EQ(hex({data, sizeof(data)}), "000100a0ff005500");
    EXPECT_EQ(hex({data, 0}), "");  // NOLINT(bugprone-string-constructor)
    EXPECT_EQ(hex({nullptr, 0}), "");
}

TEST(hex, from_hex)
{
    EXPECT_EQ(from_hex(""), bytes{});
    EXPECT_EQ(from_hex("00"), bytes{0x00});
    EXPECT_EQ(from_hex("af"), bytes{0xaf});
    EXPECT_EQ(from_hex("AF"), bytes{0xaf});
    EXPECT_EQ(from_hex("Af"), bytes{0xaf});
    EXPECT_EQ(from_hex("aF"), bytes{0xaf});
    EXPECT_EQ(from_hex("0123456789"), (bytes{0x01, 0x23, 0x45, 0x67, 0x89}));
    EXPECT_EQ(from_hex("abcdef"), (bytes{0xab, 0xcd, 0xef}));
    EXPECT_EQ(from_hex("FEDCBA"), (bytes{0xfe, 0xdc, 0xba}));
    EXPECT_EQ(from_hex("00bc01000C"), (bytes{0x00, 0xbc, 0x01, 0x00, 0x0c}));
}

TEST(hex, from_hex_odd_length)
{
    EXPECT_EQ(from_hex("0"), std::nullopt);
    EXPECT_EQ(from_hex("1"), std::nullopt);
    EXPECT_EQ(from_hex("123"), std::nullopt);
}

TEST(hex, from_hex_not_hex_digit)
{
    EXPECT_EQ(from_hex("0g"), std::nullopt);
    EXPECT_EQ(from_hex("000h"), std::nullopt);
    EXPECT_EQ(from_hex("ffffffzz"), std::nullopt);
}

TEST(hex, from_hex_0x_prefix)
{
    EXPECT_EQ(from_hex("0x"), bytes{});
    EXPECT_EQ(from_hex("0x00"), bytes{0x00});
    EXPECT_EQ(from_hex("0x01020304"), (bytes{0x01, 0x02, 0x03, 0x04}));
    EXPECT_EQ(from_hex("0x123"), std::nullopt);
    EXPECT_EQ(from_hex("00x"), std::nullopt);
    EXPECT_EQ(from_hex("00x0"), std::nullopt);
    EXPECT_EQ(from_hex("0x001y"), std::nullopt);
}

TEST(hex, from_hex_skip_whitespace)
{
    EXPECT_EQ(from_hex("0x "), bytes{});
    EXPECT_EQ(from_hex(" \n\t"), bytes{});
    EXPECT_EQ(from_hex(" \n\tab\r"), bytes{0xab});
}

TEST(hex, validate_hex)
{
    EXPECT_TRUE(validate_hex(""));
    EXPECT_TRUE(validate_hex("0x"));
    EXPECT_TRUE(validate_hex("01"));
    EXPECT_FALSE(validate_hex("0"));
    EXPECT_FALSE(validate_hex("WXYZ"));
}

TEST(hex, isspace)
{
    // Test internal isspace() compliance with std::isspace().
    // The https://en.cppreference.com/w/cpp/string/byte/isspace has the list of "space" characters.

    for (int i = int{std::numeric_limits<char>::min()}; i <= std::numeric_limits<char>::max(); ++i)
    {
        const auto c = static_cast<char>(i);
        EXPECT_EQ(evmc::internal_hex::isspace(c), (std::isspace(c) != 0));
        switch (c)
        {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            EXPECT_TRUE(evmc::internal_hex::isspace(c));
            break;
        default:
            EXPECT_FALSE(evmc::internal_hex::isspace(c));
            break;
        }
    }
}
