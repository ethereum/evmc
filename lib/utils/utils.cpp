// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/utils.hpp>
#include <stdexcept>

namespace evmc
{
bytes from_hex(std::string_view hex)
{
    if (hex.length() % 2 == 1)
        throw std::length_error{"the length of the input is odd"};

    bytes bs;
    bs.reserve(hex.length() / 2);
    int b = 0;
    for (size_t i = 0; i < hex.size(); ++i)
    {
        const auto h = hex[i];
        int v;
        if (h >= '0' && h <= '9')
            v = h - '0';
        else if (h >= 'a' && h <= 'f')
            v = h - 'a' + 10;
        else if (h >= 'A' && h <= 'F')
            v = h - 'A' + 10;
        else
            throw std::out_of_range{"not a hex digit"};

        if (i % 2 == 0)
            b = v << 4;
        else
            bs.push_back(static_cast<uint8_t>(b | v));
    }
    return bs;
}

std::string hex(bytes_view bs)
{
    std::string str;
    str.reserve(bs.size() * 2);
    for (auto b : bs)
        str += hex(b);
    return str;
}

}  // namespace evmc
