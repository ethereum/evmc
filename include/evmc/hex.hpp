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

/// Obtains a reference to the static error category object for hex errors.
const std::error_category& hex_category() noexcept;

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

/// Validates hex encoded string.
std::error_code validate_hex(std::string_view hex) noexcept;

/// Decodes hex encoded string to bytes.
///
/// Throws hex_error with the appropriate error code.
bytes from_hex(std::string_view hex);

/// Encodes bytes as hex string.
std::string hex(bytes_view bs);
}  // namespace evmc

namespace std
{
/// Template specialization of std::is_error_code_enum for evmc::hex_errc.
/// This enabled implicit conversions from evmc::hex_errc to std::error_code.
template <>
struct is_error_code_enum<evmc::hex_errc> : true_type
{};
}  // namespace std
