// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <tools/utils/utils.hpp>
#include <ostream>
#include <stdexcept>

namespace evmc
{
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
