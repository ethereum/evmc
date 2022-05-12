// EVMC: Ethereum Client-VM Connector API.
// Copyright 2021 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>

namespace evmc
{
/// String of uint8_t chars.
using bytes = std::basic_string<uint8_t>;

/// String view of uint8_t chars.
using bytes_view = std::basic_string_view<uint8_t>;

/// Hex decoding error codes.
enum class hex_errc
{
    /// Invalid hex digit encountered during decoding.
    invalid_hex_digit = 1,

    /// Input contains incomplete hex byte (length is odd).
    incomplete_hex_byte_pair = 2,
};
}  // namespace evmc

namespace std
{
/// Template specialization of std::is_error_code_enum for evmc::hex_errc.
/// This enables implicit conversions from evmc::hex_errc to std::error_code.
template <>
struct is_error_code_enum<evmc::hex_errc> : true_type
{};
}  // namespace std

namespace evmc
{

/// Obtains a reference to the static error category object for hex errors.
inline const std::error_category& hex_category() noexcept
{
    struct hex_category_impl : std::error_category
    {
        const char* name() const noexcept final { return "hex"; }

        std::string message(int ev) const final
        {
            switch (static_cast<hex_errc>(ev))
            {
            case hex_errc::invalid_hex_digit:
                return "invalid hex digit";
            case hex_errc::incomplete_hex_byte_pair:
                return "incomplete hex byte pair";
            default:
                return "unknown error";
            }
        }
    };

    // Create static category object. This involves mutex-protected dynamic initialization.
    // Because of the C++ CWG defect 253, the {} syntax is used.
    static const hex_category_impl category_instance{};

    return category_instance;
}

/// Creates error_code object out of a hex error code value.
inline std::error_code make_error_code(hex_errc errc) noexcept
{
    return {static_cast<int>(errc), hex_category()};
}

/// Hex decoding exception.
struct hex_error : std::system_error
{
    using system_error::system_error;
};

/// Encode a byte to a hex string.
inline std::string hex(uint8_t b) noexcept
{
    static constexpr auto hex_chars = "0123456789abcdef";
    return {hex_chars[b >> 4], hex_chars[b & 0xf]};
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
inline constexpr hex_errc from_hex(std::string_view hex, OutputIt result) noexcept
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
            return hex_errc::invalid_hex_digit;

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

    if (hi_nibble != empty_mark)
        return hex_errc::incomplete_hex_byte_pair;
    return {};
}
}  // namespace internal_hex

/// Validates hex encoded string.
inline std::error_code validate_hex(std::string_view hex) noexcept
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
/// Throws hex_error with the appropriate error code.
inline bytes from_hex(std::string_view hex)
{
    bytes bs;
    bs.reserve(hex.size() / 2);
    const auto ec = internal_hex::from_hex(hex, std::back_inserter(bs));
    if (static_cast<int>(ec) != 0)
        throw hex_error{ec};
    return bs;
}
}  // namespace evmc
