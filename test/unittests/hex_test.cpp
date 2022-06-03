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
    EXPECT_EQ(from_hex("000s"), std::nullopt);
    EXPECT_EQ(from_hex("00x0"), std::nullopt);
    EXPECT_EQ(from_hex("0x001x"), std::nullopt);
    EXPECT_EQ(from_hex("0x001y"), std::nullopt);
    EXPECT_EQ(from_hex("1x"), std::nullopt);
    EXPECT_EQ(from_hex("1x00"), std::nullopt);
    EXPECT_EQ(from_hex("0y"), std::nullopt);
    EXPECT_EQ(from_hex("0y00"), std::nullopt);
    EXPECT_EQ(from_hex("1y"), std::nullopt);
    EXPECT_EQ(from_hex("1y00"), std::nullopt);
}

TEST(hex, validate_hex)
{
    EXPECT_TRUE(validate_hex(""));
    EXPECT_TRUE(validate_hex("0x"));
    EXPECT_TRUE(validate_hex("01"));
    EXPECT_FALSE(validate_hex("0"));
    EXPECT_FALSE(validate_hex("WXYZ"));
}

TEST(hex, from_spaced_hex)
{
    EXPECT_EQ(from_spaced_hex("0x010203"), (bytes{0x01, 0x02, 0x03}));
    EXPECT_EQ(from_spaced_hex("0x 010203 "), (bytes{0x01, 0x02, 0x03}));
    EXPECT_EQ(from_spaced_hex(" 0 x 0 1 0 2 0 3 "), (bytes{0x01, 0x02, 0x03}));
    EXPECT_EQ(from_spaced_hex("\f 0\r x  0  1\t 0  2 \v0  3 \n"), (bytes{0x01, 0x02, 0x03}));
}

TEST(hex, from_hex_to_custom_type)
{
    struct X
    {
        uint8_t bytes[4];
    };
    constexpr auto test = [](std::string_view in) {
        return evmc::hex({evmc::from_hex<X>(in).value().bytes, sizeof(X)});
    };

    static_assert(evmc::from_hex<X>("01").value().bytes[3] == 0x01);  // Works in constexpr.

    EXPECT_EQ(test("f1f2f3f4"), "f1f2f3f4");
    EXPECT_EQ(test("01020304"), "01020304");
    EXPECT_EQ(test("010203"), "00010203");
    EXPECT_EQ(test("0102"), "00000102");
    EXPECT_EQ(test("01"), "00000001");
    EXPECT_EQ(test(""), "00000000");

    EXPECT_EQ(test("0x01020304"), "01020304");
    EXPECT_EQ(test("0x010203"), "00010203");
    EXPECT_EQ(test("0x0102"), "00000102");
    EXPECT_EQ(test("0x01"), "00000001");
    EXPECT_EQ(test("0x"), "00000000");

    EXPECT_FALSE(evmc::from_hex<X>("0"));
    EXPECT_FALSE(evmc::from_hex<X>("1"));
    EXPECT_FALSE(evmc::from_hex<X>("0x "));
    EXPECT_FALSE(evmc::from_hex<X>("0xf"));
    EXPECT_FALSE(evmc::from_hex<X>("0x 00"));
    EXPECT_FALSE(evmc::from_hex<X>("1x"));
    EXPECT_FALSE(evmc::from_hex<X>("1x00"));
    EXPECT_FALSE(evmc::from_hex<X>("fx"));
    EXPECT_FALSE(evmc::from_hex<X>("fx00"));

    // The result type is too small for the input.
    EXPECT_FALSE(evmc::from_hex<X>("0000000000"));
    EXPECT_FALSE(evmc::from_hex<X>("0x0000000000"));
}
