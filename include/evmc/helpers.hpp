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

/** @} */
