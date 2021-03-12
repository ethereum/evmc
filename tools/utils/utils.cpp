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
struct hex_error_category : std::error_category
{
    const char* name() const noexcept override { return "hex"; }

    std::string message(int ev) const override
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

inline int from_hex_digit(char h)
{
    if (h >= '0' && h <= '9')
        return h - '0';
    else if (h >= 'a' && h <= 'f')
        return h - 'a' + 10;
    else if (h >= 'A' && h <= 'F')
        return h - 'A' + 10;
    else
        throw hex_error{hex_errc::invalid_hex_digit};
}

template <typename OutputIt>
inline void from_hex(const char* hex, size_t size, OutputIt result)
{
    // TODO: This can be implemented with hex_decode_iterator and std::copy.

    const auto hex_end = hex + size;

    // Omit the optional 0x prefix.
    if (size >= 2 && hex[0] == '0' && hex[1] == 'x')
        hex += 2;

    constexpr int empty_byte_mark = -1;
    int b = empty_byte_mark;
    for (auto it = hex; it != hex_end; ++it)
    {
        const auto h = *it;
        if (std::isspace(h))
            continue;

        const int v = from_hex_digit(h);
        if (b == empty_byte_mark)
        {
            b = v << 4;
        }
        else
        {
            *result++ = static_cast<uint8_t>(b | v);
            b = empty_byte_mark;
        }
    }

    if (b != empty_byte_mark)
        throw hex_error{hex_errc::incomplete_hex_byte_pair};
}
}  // namespace

std::error_code make_error_code(hex_errc errc) noexcept
{
    static hex_error_category category;
    return {static_cast<int>(errc), category};
}

std::error_code validate_hex(const std::string& hex) noexcept
{
    struct noop_output_iterator
    {
        uint8_t sink = {};
        uint8_t& operator*() noexcept { return sink; }
        noop_output_iterator operator++(int) noexcept { return *this; }
    };

    try
    {
        from_hex(hex.data(), hex.size(), noop_output_iterator{});
        return {};
    }
    catch (const hex_error& e)
    {
        return e.code();
    }
}

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
