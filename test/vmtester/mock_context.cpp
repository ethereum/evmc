#include <evmc/evmc.h>
#include <evmc/helpers.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

evmc_uint256be balance(evmc_context* context, const evmc_address* address)
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

static void call(evmc_result* result, evmc_context* context, const evmc_message* msg)
{
    (void)context;
    (void)msg;
    result->status_code = EVMC_FAILURE;
}

static void get_tx_context(evmc_tx_context* result, evmc_context* context)
{
    (void)result;
    (void)context;
}

static void get_block_hash(evmc_uint256be* result, evmc_context* context, int64_t number)
{
    (void)result;
    (void)context;
    (void)number;
}

/// EVM log callback.
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

extern const evmc_context_fn_table mock_context_fn_table = {
    account_exists, get_storage,  set_storage, get_balance,    get_code_size,  get_code_hash,
    copy_code,      selfdestruct, call,        get_tx_context, get_block_hash, emit_log,
};
