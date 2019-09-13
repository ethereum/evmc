/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#include "example_precompiles_vm.h"
#include <algorithm>

static evmc_result execute_identity(const evmc_message* msg)
{
    auto result = evmc_result{};

    // Check the gas cost.
    auto gas_cost = 15 + 3 * ((int64_t(msg->input_size) + 31) / 32);
    auto gas_left = msg->gas - gas_cost;
    if (gas_left < 0)
    {
        result.status_code = EVMC_OUT_OF_GAS;
        return result;
    }

    // Execute.
    auto data = new uint8_t[msg->input_size];
    std::copy_n(msg->input_data, msg->input_size, data);

    // Return the result.
    result.status_code = EVMC_SUCCESS;
    result.output_data = data;
    result.output_size = msg->input_size;
    result.release = [](const evmc_result* r) { delete[] r->output_data; };
    result.gas_left = gas_left;
    return result;
}

static evmc_result execute_empty(const evmc_message* msg)
{
    auto result = evmc_result{};
    result.status_code = EVMC_SUCCESS;
    result.gas_left = msg->gas;
    return result;
}

static evmc_result not_implemented()
{
    auto result = evmc_result{};
    result.status_code = EVMC_REJECTED;
    return result;
}

static evmc_result execute(evmc_vm*,
                           const evmc_host_interface*,
                           evmc_host_context*,
                           enum evmc_revision rev,
                           const evmc_message* msg,
                           const uint8_t* /*code*/,
                           size_t /*code_size*/)
{
    // The EIP-1352 (https://eips.ethereum.org/EIPS/eip-1352) defines
    // the range 0 - 0xffff (2 bytes) of addresses reserved for precompiled contracts.
    // Check if the destination address is within the reserved range.

    constexpr auto prefix_size = sizeof(evmc_address) - 2;
    const auto& dst = msg->destination;
    // Check if the address prefix is all zeros.
    if (std::any_of(&dst.bytes[0], &dst.bytes[prefix_size], [](uint8_t x) { return x != 0; }))
    {
        // If not, reject the execution request.
        auto result = evmc_result{};
        result.status_code = EVMC_REJECTED;
        return result;
    }

    // Extract the precompiled contract id from last 2 bytes of the destination address.
    const auto id = (dst.bytes[prefix_size] << 8) | dst.bytes[prefix_size + 1];
    switch (id)
    {
    case 0x0001:  // ECDSARECOVER
    case 0x0002:  // SHA256
    case 0x0003:  // RIPEMD160
        return not_implemented();

    case 0x0004:  // Identity
        return execute_identity(msg);

    case 0x0005:  // EXPMOD
    case 0x0006:  // SNARKV
    case 0x0007:  // BNADD
    case 0x0008:  // BNMUL
        if (rev < EVMC_BYZANTIUM)
            return execute_empty(msg);
        return not_implemented();

    default:  // As if empty code was executed.
        return execute_empty(msg);
    }
}

evmc_vm* evmc_create_example_precompiles_vm()
{
    static struct evmc_vm vm = {
        EVMC_ABI_VERSION,
        "example_precompiles_vm",
        PROJECT_VERSION,
        [](evmc_vm*) {},
        execute,
        [](evmc_vm*) { return evmc_capabilities_flagset{EVMC_CAPABILITY_PRECOMPILES}; },
        nullptr,
    };
    return &vm;
}
