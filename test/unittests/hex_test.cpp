// EVMC: Ethereum Client-VM Connector API.
// Copyright 2021 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/hex.hpp>
#include <gtest/gtest.h>

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

static std::error_code catch_hex_error(const char* input)
{
    try
    {
        from_hex(input);
    }
    catch (const hex_error& e)
    {
        return e.code();
    }
    return {};
}

TEST(hex, from_hex_odd_length)
{
    EXPECT_EQ(catch_hex_error("0"), hex_errc::incomplete_hex_byte_pair);
    EXPECT_EQ(catch_hex_error("1"), hex_errc::incomplete_hex_byte_pair);
    EXPECT_EQ(catch_hex_error("123"), hex_errc::incomplete_hex_byte_pair);
}

TEST(hex, from_hex_not_hex_digit)
{
    EXPECT_EQ(catch_hex_error("0g"), hex_errc::invalid_hex_digit);
    EXPECT_EQ(catch_hex_error("000h"), hex_errc::invalid_hex_digit);
    EXPECT_EQ(catch_hex_error("ffffffzz"), hex_errc::invalid_hex_digit);
}

TEST(hex, from_hex_0x_prefix)
{
    EXPECT_EQ(from_hex("0x"), bytes{});
    EXPECT_EQ(from_hex("0x00"), bytes{0x00});
    EXPECT_EQ(from_hex("0x01020304"), (bytes{0x01, 0x02, 0x03, 0x04}));
    EXPECT_EQ(catch_hex_error("0x123"), hex_errc::incomplete_hex_byte_pair);
    EXPECT_EQ(catch_hex_error("00x"), hex_errc::invalid_hex_digit);
    EXPECT_EQ(catch_hex_error("00x0"), hex_errc::invalid_hex_digit);
    EXPECT_EQ(catch_hex_error("0x001y"), hex_errc::invalid_hex_digit);
}

TEST(hex, from_hex_skip_whitespace)
{
    EXPECT_EQ(from_hex("0x "), bytes{});
    EXPECT_EQ(from_hex(" \n\t"), bytes{});
    EXPECT_EQ(from_hex(" \n\tab\r"), bytes{0xab});
}

TEST(hex, validate_hex)
{
    EXPECT_FALSE(validate_hex(""));
    EXPECT_FALSE(validate_hex("0x"));
    EXPECT_FALSE(validate_hex("01"));
    EXPECT_EQ(validate_hex("0"), hex_errc::incomplete_hex_byte_pair);
    EXPECT_EQ(validate_hex("WXYZ"), hex_errc::invalid_hex_digit);
}

TEST(hex, hex_error_code)
{
    std::error_code ec1 = hex_errc::invalid_hex_digit;
    EXPECT_EQ(ec1.value(), 1);
    EXPECT_EQ(ec1.message(), "invalid hex digit");

    std::error_code ec2 = hex_errc::incomplete_hex_byte_pair;
    EXPECT_EQ(ec2.value(), 2);
    EXPECT_EQ(ec2.message(), "incomplete hex byte pair");
}

TEST(hex, hex_category_inspection)
{
    EXPECT_STREQ(hex_category().name(), "hex");
}

TEST(hex, hex_category_comparison)
{
    std::error_code ec1 = hex_errc::invalid_hex_digit;
    EXPECT_EQ(ec1.category(), hex_category());

    std::error_code ec2 = hex_errc::incomplete_hex_byte_pair;
    EXPECT_EQ(ec2.category(), hex_category());
}
