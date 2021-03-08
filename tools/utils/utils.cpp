// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <tools/utils/utils.hpp>
#include <ostream>
#include <stdexcept>

namespace evmc
{
namespace
{
inline int from_hex_digit(char h)
{
    if (h >= '0' && h <= '9')
        return h - '0';
    else if (h >= 'a' && h <= 'f')
        return h - 'a' + 10;
    else if (h >= 'A' && h <= 'F')
        return h - 'A' + 10;
    else
        throw std::out_of_range{"not a hex digit"};
}

template <typename OutputIt>
inline void from_hex(const char* hex, size_t size, OutputIt result)
{
    // Omit the optional 0x prefix.
    if (size >= 2 && hex[0] == '0' && hex[1] == 'x')
    {
        hex += 2;
        size -= 2;
    }

    if (size % 2 == 1)
        throw std::length_error{"the length of the input is odd"};

    int b = 0;
    for (size_t i = 0; i < size; ++i)
    {
        const int v = from_hex_digit(hex[i]);
        if (i % 2 == 0)
            b = v << 4;
        else
            *result++ = static_cast<uint8_t>(b | v);
    }
}
}  // namespace

bytes from_hex(const std::string& hex)
{
    bytes bs;
    bs.reserve(hex.size() / 2);
    from_hex(hex.data(), hex.size(), std::back_inserter(bs));
    return bs;
}

std::string hex(const uint8_t* data, size_t size)
{
    std::string str;
    str.reserve(size * 2);
    for (const auto end = data + size; data != end; ++data)
        str += hex(*data);
    return str;
}

std::ostream& operator<<(std::ostream& os, evmc_status_code status_code)
{
    const char* s = nullptr;
    switch (status_code)
    {
    case EVMC_SUCCESS:
        s = "success";
        break;
    case EVMC_FAILURE:
        s = "failure";
        break;
    case EVMC_REVERT:
        s = "revert";
        break;
    case EVMC_OUT_OF_GAS:
        s = "out of gas";
        break;
    case EVMC_INVALID_INSTRUCTION:
        s = "invalid instruction";
        break;
    case EVMC_UNDEFINED_INSTRUCTION:
        s = "undefined instruction";
        break;
    case EVMC_STACK_OVERFLOW:
        s = "stack overflow";
        break;
    case EVMC_STACK_UNDERFLOW:
        s = "stack underflow";
        break;
    case EVMC_BAD_JUMP_DESTINATION:
        s = "bad jump destination";
        break;
    case EVMC_INVALID_MEMORY_ACCESS:
        s = "invalid memory access";
        break;
    case EVMC_CALL_DEPTH_EXCEEDED:
        s = "call depth exceeded";
        break;
    case EVMC_STATIC_MODE_VIOLATION:
        s = "static mode violation";
        break;
    case EVMC_PRECOMPILE_FAILURE:
        s = "precompile failure";
        break;
    case EVMC_CONTRACT_VALIDATION_FAILURE:
        s = "contract validation failure";
        break;
    case EVMC_ARGUMENT_OUT_OF_RANGE:
        s = "argument out of range";
        break;
    case EVMC_WASM_UNREACHABLE_INSTRUCTION:
        s = "wasm unreachable instruction";
        break;
    case EVMC_WASM_TRAP:
        s = "wasm trap";
        break;
    case EVMC_INSUFFICIENT_BALANCE:
        s = "insufficient balance";
        break;
    case EVMC_INTERNAL_ERROR:
        s = "internal error";
        break;
    case EVMC_REJECTED:
        s = "rejected";
        break;
    case EVMC_OUT_OF_MEMORY:
        s = "out of memory";
        break;
    default:
        throw std::invalid_argument{"invalid EVMC status code: " + std::to_string(status_code)};
    }
    return os << s;
}

std::ostream& operator<<(std::ostream& os, evmc_revision revision)
{
    const char* s = nullptr;
    switch (revision)
    {
    case EVMC_FRONTIER:
        s = "Frontier";
        break;
    case EVMC_HOMESTEAD:
        s = "Homestead";
        break;
    case EVMC_TANGERINE_WHISTLE:
        s = "Tangerine Whistle";
        break;
    case EVMC_SPURIOUS_DRAGON:
        s = "Spurious Dragon";
        break;
    case EVMC_BYZANTIUM:
        s = "Byzantium";
        break;
    case EVMC_CONSTANTINOPLE:
        s = "Constantinople";
        break;
    case EVMC_PETERSBURG:
        s = "Petersburg";
        break;
    case EVMC_ISTANBUL:
        s = "Istanbul";
        break;
    case EVMC_BERLIN:
        s = "Berlin";
        break;
    default:
        throw std::invalid_argument{"invalid EVM revision: " + std::to_string(revision)};
    }
    return os << s;
}

}  // namespace evmc
