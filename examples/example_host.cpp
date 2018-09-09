// EVMC -- Ethereum Client-VM Connector API
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

/// @file
/// Example implementation of an EVMC Host.

#include "example_host.h"

#include <evmc/helpers.h>
#include <evmc/helpers.hpp>

#include <map>

struct account
{
    evmc_uint256be balance = {};
    size_t code_size = 0;
    evmc_bytes32 code_hash = {};
    std::map<evmc_bytes32, evmc_bytes32> storage;
};

struct example_host_context : evmc_context
{
    example_host_context();

    evmc_tx_context tx_context = {};

    std::map<evmc_address, account> accounts;
};

static bool account_exists(evmc_context* context, const evmc_address* address)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    return host->accounts.find(*address) != host->accounts.end();
}

static evmc_bytes32 get_storage(evmc_context* context,
                                const evmc_address* address,
                                const evmc_bytes32* key)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
        return it->second.storage[*key];
    return {};
}

static enum evmc_storage_status set_storage(evmc_context* context,
                                            const evmc_address* address,
                                            const evmc_bytes32* key,
                                            const evmc_bytes32* value)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto& account = host->accounts[*address];
    auto prevValue = account.storage[*key];
    account.storage[*key] = *value;

    if (prevValue == *value)
        return EVMC_STORAGE_UNCHANGED;
    else
        return EVMC_STORAGE_MODIFIED;
}

static evmc_uint256be get_balance(evmc_context* context, const evmc_address* address)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
        return it->second.balance;
    return {};
}

static size_t get_code_size(evmc_context* context, const evmc_address* address)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
        return it->second.code_size;
    return 0;
}

static evmc_bytes32 get_code_hash(evmc_context* context, const evmc_address* address)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    auto it = host->accounts.find(*address);
    if (it != host->accounts.end())
        return it->second.code_hash;
    return {};
}

static size_t copy_code(evmc_context* context,
                        const evmc_address* address,
                        size_t code_offset,
                        uint8_t* buffer_data,
                        size_t buffer_size)
{
    (void)context;
    (void)address;
    (void)code_offset;
    (void)buffer_data;
    (void)buffer_size;
    return 0;
}

static void selfdestruct(evmc_context* context,
                         const evmc_address* address,
                         const evmc_address* beneficiary)
{
    (void)context;
    (void)address;
    (void)beneficiary;
}

static evmc_result call(evmc_context* context, const evmc_message* msg)
{
    (void)context;
    (void)msg;
    evmc_result result{};
    result.status_code = EVMC_FAILURE;
    return result;
}

static evmc_tx_context get_tx_context(evmc_context* context)
{
    (void)context;
    evmc_tx_context result{};
    return result;
}

static evmc_bytes32 get_block_hash(evmc_context* context, int64_t number)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    int64_t current_block_number = host->tx_context.block_number;

    evmc_bytes32 example_block_hash;
    if (number < current_block_number && number >= current_block_number - 256)
        example_block_hash = {{1, 1, 1, 1}};
    else
        example_block_hash = {};
    return example_block_hash;
}

static void emit_log(evmc_context* context,
                     const evmc_address* address,
                     const uint8_t* data,
                     size_t data_size,
                     const evmc_bytes32 topics[],
                     size_t topics_count)
{
    (void)context;
    (void)address;
    (void)data;
    (void)data_size;
    (void)topics;
    (void)topics_count;
}

static const evmc_host_interface interface = {
    account_exists, get_storage,  set_storage, get_balance,    get_code_size,  get_code_hash,
    copy_code,      selfdestruct, call,        get_tx_context, get_block_hash, emit_log,
};

example_host_context::example_host_context() : evmc_context{&interface} {}

extern "C" {

evmc_context* example_host_create_context()
{
    return new example_host_context;
}

void example_host_destroy_context(evmc_context* context)
{
    delete context;
}
}
