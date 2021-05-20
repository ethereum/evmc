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
const char* to_string(evmc_status_code status_code) noexcept
{
    switch (status_code)
    {
    case EVMC_SUCCESS:
        return "success";
    case EVMC_FAILURE:
        return "failure";
    case EVMC_REVERT:
        return "revert";
    case EVMC_OUT_OF_GAS:
        return "out of gas";
    case EVMC_INVALID_INSTRUCTION:
        return "invalid instruction";
    case EVMC_UNDEFINED_INSTRUCTION:
        return "undefined instruction";
    case EVMC_STACK_OVERFLOW:
        return "stack overflow";
    case EVMC_STACK_UNDERFLOW:
        return "stack underflow";
    case EVMC_BAD_JUMP_DESTINATION:
        return "bad jump destination";
    case EVMC_INVALID_MEMORY_ACCESS:
        return "invalid memory access";
    case EVMC_CALL_DEPTH_EXCEEDED:
        return "call depth exceeded";
    case EVMC_STATIC_MODE_VIOLATION:
        return "static mode violation";
    case EVMC_PRECOMPILE_FAILURE:
        return "precompile failure";
    case EVMC_CONTRACT_VALIDATION_FAILURE:
        return "contract validation failure";
    case EVMC_ARGUMENT_OUT_OF_RANGE:
        return "argument out of range";
    case EVMC_WASM_UNREACHABLE_INSTRUCTION:
        return "wasm unreachable instruction";
    case EVMC_WASM_TRAP:
        return "wasm trap";
    case EVMC_INSUFFICIENT_BALANCE:
        return "insufficient balance";
    case EVMC_INTERNAL_ERROR:
        return "internal error";
    case EVMC_REJECTED:
        return "rejected";
    case EVMC_OUT_OF_MEMORY:
        return "out of memory";
    }
    return "<unknown>";
}

const char* to_string(evmc_revision rev) noexcept
{
    switch (rev)
    {
    case EVMC_FRONTIER:
        return "Frontier";
    case EVMC_HOMESTEAD:
        return "Homestead";
    case EVMC_TANGERINE_WHISTLE:
        return "Tangerine Whistle";
    case EVMC_SPURIOUS_DRAGON:
        return "Spurious Dragon";
    case EVMC_BYZANTIUM:
        return "Byzantium";
    case EVMC_CONSTANTINOPLE:
        return "Constantinople";
    case EVMC_PETERSBURG:
        return "Petersburg";
    case EVMC_ISTANBUL:
        return "Istanbul";
    case EVMC_BERLIN:
        return "Berlin";
    case EVMC_LONDON:
        return "London";
    }
    return "<unknown>";
}
}  // namespace

std::ostream& operator<<(std::ostream& os, evmc_status_code status_code)
{
    return os << to_string(status_code);
}

std::ostream& operator<<(std::ostream& os, evmc_revision rev)
{
    return os << to_string(rev);
}

}  // namespace evmc
