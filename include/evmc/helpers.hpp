/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */

/**
 * EVMC C++ helpers
 *
 * A collection of helpers (overloaded operators) for using EVMC effectively in C++.
 *
 * @defgroup cpphelpers EVMC C++ helpers
 * @{
 */
#pragma once

#include <evmc/evmc.h>

#include <cstring>
#include <functional>

/// The comparator for std::map<evmc_address, ...>.
bool operator<(const evmc_address& a, const evmc_address& b)
{
    return std::memcmp(a.bytes, b.bytes, sizeof(a)) < 0;
}

/// The comparator for std::map<evmc_bytes32, ...>.
bool operator<(const evmc_bytes32& a, const evmc_bytes32& b)
{
    return std::memcmp(a.bytes, b.bytes, sizeof(a)) < 0;
}

/// The comparator for equality.
bool operator==(const evmc_address& a, const evmc_address& b)
{
    return std::memcmp(a.bytes, b.bytes, sizeof(a)) == 0;
}

/// The comparator for equality.
bool operator==(const evmc_bytes32& a, const evmc_bytes32& b)
{
    return std::memcmp(a.bytes, b.bytes, sizeof(a)) == 0;
}

/// FNV1a hash function with 64-bit result.
static inline uint64_t fnv1a_64(const uint8_t* ptr, size_t len)
{
    constexpr uint64_t prime = 1099511628211ULL;
    constexpr uint64_t offset_basis = 14695981039346656037ULL;

    uint64_t ret = offset_basis;
    for (size_t i = 0; i < len; i++)
    {
        ret ^= ptr[i];
        ret *= prime;
    }
    return ret;
}

static_assert(sizeof(size_t) == sizeof(uint64_t), "size_t is not 64-bit wide");

namespace std
{
/// Hash operator template needed for std::ordered_set and others using hashes.
template <>
struct hash<evmc_bytes32>
{
    /// The argument type.
    typedef evmc_bytes32 argument_type;
    /// The result type.
    typedef std::size_t result_type;
    /// Hash operator using FNV1a.
    result_type operator()(argument_type const& s) const noexcept
    {
        return fnv1a_64(s.bytes, sizeof(s.bytes));
    }
};
}  // namespace std

/** @} */
