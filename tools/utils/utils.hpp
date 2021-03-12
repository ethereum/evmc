// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <evmc/evmc.h>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <system_error>

namespace evmc
{
using bytes = std::basic_string<uint8_t>;

enum class hex_errc
{
    invalid_hex_digit = 1,
    incomplete_hex_byte_pair = 2,
    unknown_error = -1
};

std::error_code make_error_code(hex_errc errc) noexcept;

/// Encode a byte to a hex string.
inline std::string hex(uint8_t b) noexcept
{
    static constexpr auto hex_chars = "0123456789abcdef";
    return {hex_chars[b >> 4], hex_chars[b & 0xf]};
}

/// Validates hex encoded string.
std::error_code validate_hex(const std::string& hex) noexcept;

/// Decodes hex encoded string to bytes.
///
/// Exceptions:
/// - std::length_error when the input has invalid length (must be even).
/// - std::out_of_range when invalid hex digit encountered.
bytes from_hex(const std::string& hex);

/// Encodes bytes as hex string.
std::string hex(const uint8_t* data, size_t size);


/// Output stream operator for evmc_status_code.
std::ostream& operator<<(std::ostream& os, evmc_status_code status_code);

/// Output stream operator for EVM revision.
std::ostream& operator<<(std::ostream& os, evmc_revision revision);

}  // namespace evmc

namespace std
{
template <>
struct is_error_code_enum<evmc::hex_errc> : true_type
{};
}  // namespace std
