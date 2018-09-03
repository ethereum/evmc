// EVMC -- Ethereum Client-VM Connector API
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

/// @file
/// Example implementation of an EVMC Host.

#include "example_host.h"

#include <evmc/helpers.h>

struct example_host_context : evmc_context
{
    example_host_context();

    evmc_tx_context tx_context = {};
};

static evmc_uint256be balance(evmc_context* context, const evmc_address* address)
{
    (void)context;
    (void)address;
    evmc_uint256be ret = {{1, 2, 3, 4}};
    return ret;
}

static int account_exists(evmc_context* context, const evmc_address* address)
{
    (void)context;
    (void)address;
    return 0;
}

static void get_storage(evmc_uint256be* result,
                        evmc_context* context,
                        const evmc_address* address,
                        const evmc_uint256be* key)
{
    (void)result;
    (void)context;
    (void)address;
    (void)key;
}

static enum evmc_storage_status set_storage(evmc_context* context,
                                            const evmc_address* address,
                                            const evmc_uint256be* key,
                                            const evmc_uint256be* value)
{
    (void)context;
    (void)address;
    (void)key;
    (void)value;
    return EVMC_STORAGE_UNCHANGED;
}

static void get_balance(evmc_uint256be* result, evmc_context* context, const evmc_address* address)
{
    *result = balance(context, address);
}

static size_t get_code_size(evmc_context* context, const evmc_address* address)
{
    (void)context;
    (void)address;
    return 0;
}

static void get_code_hash(evmc_uint256be* result,
                          evmc_context* context,
                          const evmc_address* address)
{
    (void)result;
    (void)context;
    (void)address;
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

static int get_block_hash(evmc_uint256be* result, evmc_context* context, int64_t number)
{
    example_host_context* host = static_cast<example_host_context*>(context);
    int64_t current_block_number = host->tx_context.block_number;

    if (number >= current_block_number || number < current_block_number - 256)
        return 0;

    evmc_uint256be example_block_hash{};
    *result = example_block_hash;
    return 1;
}

static void emit_log(evmc_context* context,
                     const evmc_address* address,
                     const uint8_t* data,
                     size_t data_size,
                     const evmc_uint256be topics[],
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
