// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <evmc/evmc.h>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace evmc
{
using bytes = std::basic_string<uint8_t>;

/// Encode a byte to a hex string.
inline std::string hex(uint8_t b) noexcept
{
    static constexpr auto hex_chars = "0123456789abcdef";
    return {hex_chars[b >> 4], hex_chars[b & 0xf]};
}

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
