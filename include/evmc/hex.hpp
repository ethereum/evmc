// EVMC: Ethereum Client-VM Connector API.
// Copyright 2021 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <cstdint>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>

namespace evmc
{
/// String of uint8_t chars.
using bytes = std::basic_string<uint8_t>;

/// String view of uint8_t chars.
using bytes_view = std::basic_string_view<uint8_t>;


/// Encode a byte to a hex string.
inline std::string hex(uint8_t b) noexcept
{
    static constexpr auto hex_digits = "0123456789abcdef";
    return {hex_digits[b >> 4], hex_digits[b & 0xf]};
}

/// Encodes bytes as hex string.
inline std::string hex(bytes_view bs)
{
    std::string str;
    str.reserve(bs.size() * 2);
    for (const auto b : bs)
        str += hex(b);
    return str;
}

namespace internal_hex
{
/// Extracts the nibble value out of a hex digit.
/// Returns -1 in case of invalid hex digit.
inline constexpr int from_hex_digit(char h) noexcept
{
    if (h >= '0' && h <= '9')
        return h - '0';
    else if (h >= 'a' && h <= 'f')
        return h - 'a' + 10;
    else if (h >= 'A' && h <= 'F')
        return h - 'A' + 10;
    else
        return -1;
}

/// The constexpr variant of std::isspace().
inline constexpr bool isspace(char ch) noexcept
{
    // Implementation taken from LLVM's libc.
    return ch == ' ' || (static_cast<unsigned>(ch) - '\t') < 5;
}

template <typename OutputIt>
inline constexpr bool from_hex(std::string_view hex, OutputIt result) noexcept
{
    // Omit the optional 0x prefix.
    if (hex.size() >= 2 && hex[0] == '0' && hex[1] == 'x')
        hex.remove_prefix(2);

    constexpr int empty_mark = -1;
    int hi_nibble = empty_mark;
    for (const auto h : hex)
    {
        if (isspace(h))
            continue;

        const int v = from_hex_digit(h);
        if (v < 0)
            return false;

        if (hi_nibble == empty_mark)
        {
            hi_nibble = v << 4;
        }
        else
        {
            *result++ = static_cast<uint8_t>(hi_nibble | v);
            hi_nibble = empty_mark;
        }
    }

    return hi_nibble == empty_mark;
}
}  // namespace internal_hex

/// Validates hex encoded string.
///
/// @return  True if the input is valid hex.
inline bool validate_hex(std::string_view hex) noexcept
{
    struct noop_output_iterator
    {
        uint8_t sink = {};
        uint8_t& operator*() noexcept { return sink; }
        noop_output_iterator operator++(int) noexcept { return *this; }  // NOLINT(cert-dcl21-cpp)
    };

    return internal_hex::from_hex(hex, noop_output_iterator{});
}

/// Decodes hex encoded string to bytes.
///
/// In case the input is invalid the returned value is std::nullopt.
/// This can happen if a non-hex digit or odd number of digits is encountered.
/// Whitespace in the input is ignored.
inline std::optional<bytes> from_hex(std::string_view hex)
{
    bytes bs;
    bs.reserve(hex.size() / 2);
    if (!internal_hex::from_hex(hex, std::back_inserter(bs)))
        return {};
    return bs;
}
}  // namespace evmc
