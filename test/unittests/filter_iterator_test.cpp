// EVMC: Ethereum Client-VM Connector API.
// Copyright 2022 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/filter_iterator.hpp>
#include <gtest/gtest.h>
#include <cctype>

using evmc::skip_space_iterator;

namespace
{
std::string remove_space(std::string_view in)
{
    // Copy input to additional buffer. This helps with out-of-buffer reads detection by sanitizers.
    const std::vector<char> in_buffer(in.begin(), in.end());

    // Filter the input.
    std::string out;
    std::copy(skip_space_iterator{in_buffer.begin(), in_buffer.end()},
              skip_space_iterator{in_buffer.end(), in_buffer.end()}, std::back_inserter(out));
    return out;
}

bool is_positive(int x) noexcept
{
    return x > 0;
}
}  // namespace


TEST(filter_iterator, filter_positive_integers)
{
    std::vector<int> in{1, 0, 0, 2, -3, 3, 4, 5, 0, 6, 7, -1, -2, 0, 8, 9, -10};
    std::vector<int> out;

    using iter = evmc::filter_iterator<std::vector<int>::const_iterator, is_positive>;
    std::copy(iter{in.begin(), in.end()}, iter{in.end(), in.end()}, std::back_inserter(out));
    ASSERT_EQ(out.size(), 9u);
    EXPECT_EQ(out[0], 1);
    EXPECT_EQ(out[1], 2);
    EXPECT_EQ(out[2], 3);
    EXPECT_EQ(out[3], 4);
    EXPECT_EQ(out[4], 5);
    EXPECT_EQ(out[5], 6);
    EXPECT_EQ(out[6], 7);
    EXPECT_EQ(out[7], 8);
    EXPECT_EQ(out[8], 9);
}


TEST(skip_space_iterator, empty)
{
    EXPECT_EQ(remove_space(""), "");
    EXPECT_EQ(remove_space(" "), "");
    EXPECT_EQ(remove_space("  "), "");
}

TEST(skip_space_iterator, filter_middle)
{
    EXPECT_EQ(remove_space("x y"), "xy");
    EXPECT_EQ(remove_space("x  y"), "xy");
}

TEST(skip_space_iterator, filter_front)
{
    EXPECT_EQ(remove_space(" x"), "x");
    EXPECT_EQ(remove_space("  x"), "x");
}

TEST(skip_space_iterator, filter_back)
{
    EXPECT_EQ(remove_space("x "), "x");
    EXPECT_EQ(remove_space("x  "), "x");
}

TEST(skip_space_iterator, filter_mixed)
{
    EXPECT_EQ(remove_space(" x y z "), "xyz");
    EXPECT_EQ(remove_space("  x  y  z  "), "xyz");
}

TEST(skip_space_iterator, isspace)
{
    // Test internal isspace() compliance with std::isspace().
    // The https://en.cppreference.com/w/cpp/string/byte/isspace has the list of "space" characters.

    for (int i = int{std::numeric_limits<char>::min()}; i <= std::numeric_limits<char>::max(); ++i)
    {
        const auto c = static_cast<char>(i);
        EXPECT_EQ(evmc::isspace(c), (std::isspace(c) != 0));
        switch (c)
        {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            EXPECT_TRUE(evmc::isspace(c));
            break;
        default:
            EXPECT_FALSE(evmc::isspace(c));
            break;
        }
    }
}
