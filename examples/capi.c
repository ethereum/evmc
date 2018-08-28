#include <evmc/evmc.h>
#include <evmc/helpers.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "examplevm/examplevm.h"


struct evmc_uint256be balance(struct evmc_context* context, const struct evmc_address* address)
{
    (void)context;
    (void)address;
    struct evmc_uint256be ret = {.bytes = {1, 2, 3, 4}};
    return ret;
}

static void print_address(const struct evmc_address* address)
{
    int i = 0;
    for (i = 0; i < sizeof(address->bytes); ++i)
        printf("%x", address->bytes[i] & 0xff);
}

static int account_exists(struct evmc_context* context, const struct evmc_address* address)
{
    (void)context;
    printf("EVM-C: EXISTS @");
    print_address(address);
    printf("\n");
    return 0;
}

static void get_storage(struct evmc_uint256be* result,
                        struct evmc_context* context,
                        const struct evmc_address* address,
                        const struct evmc_uint256be* key)
{
    (void)result;
    (void)context;
    (void)key;
    printf("EVM-C: SLOAD @");
    print_address(address);
    printf("\n");
}

static enum evmc_storage_status set_storage(struct evmc_context* context,
                                            const struct evmc_address* address,
                                            const struct evmc_uint256be* key,
                                            const struct evmc_uint256be* value)
{
    (void)context;
    (void)key;
    (void)value;
    printf("EVM-C: SSTORE @");
    print_address(address);
    printf("\n");
    return EVMC_STORAGE_UNCHANGED;
}

static void get_balance(struct evmc_uint256be* result,
                        struct evmc_context* context,
                        const struct evmc_address* address)
{
    printf("EVM-C: BALANCE @");
    print_address(address);
    printf("\n");
    *result = balance(context, address);
}

static size_t get_code_size(struct evmc_context* context, const struct evmc_address* address)
{
    (void)context;
    printf("EVM-C: CODESIZE @");
    print_address(address);
    printf("\n");
    return 0;
}

static void get_code_hash(struct evmc_uint256be* result,
                          struct evmc_context* context,
                          const struct evmc_address* address)
{
    (void)result;
    (void)context;
    printf("EVM-C: CODEHASH @");
    print_address(address);
    printf("\n");
}

static size_t copy_code(struct evmc_context* context,
                        const struct evmc_address* address,
                        size_t code_offset,
                        uint8_t* buffer_data,
                        size_t buffer_size)
{
    (void)context;
    (void)code_offset;
    (void)buffer_data;
    (void)buffer_size;
    printf("EVM-C: COPYCODE @");
    print_address(address);
    printf("\n");
    return 0;
}

static void selfdestruct(struct evmc_context* context,
                         const struct evmc_address* address,
                         const struct evmc_address* beneficiary)
{
    (void)context;
    printf("EVM-C: SELFDESTRUCT ");
    print_address(address);
    printf(" -> ");
    print_address(beneficiary);
    printf("\n");
}

static void call(struct evmc_result* result,
                 struct evmc_context* context,
                 const struct evmc_message* msg)
{
    (void)context;
    printf("EVM-C: CALL (depth: %d)\n", msg->depth);
    result->status_code = EVMC_FAILURE;
}

static void get_tx_context(struct evmc_tx_context* result, struct evmc_context* context)
{
    (void)result;
    (void)context;
}

static void get_block_hash(struct evmc_uint256be* result,
                           struct evmc_context* context,
                           int64_t number)
{
    (void)result;
    (void)context;
    (void)number;
}

/// EVM log callback.
static void emit_log(struct evmc_context* context,
                     const struct evmc_address* address,
                     const uint8_t* data,
                     size_t data_size,
                     const struct evmc_uint256be topics[],
                     size_t topics_count)
{
    (void)context;
    (void)address;
    (void)data;
    (void)data_size;
    (void)topics;
    printf("EVM-C: LOG%d\n", (int)topics_count);
}

static const struct evmc_context_fn_table ctx_fn_table = {
    account_exists, get_storage,  set_storage, get_balance,    get_code_size,  get_code_hash,
    copy_code,      selfdestruct, call,        get_tx_context, get_block_hash, emit_log,
};

/// Example how the API is supposed to be used.
int main()
{
    struct evmc_instance* vm = evmc_create_examplevm();
    if (!evmc_is_abi_compatible(vm))
        return 1;

    // EVM bytecode goes here. This is one of the examples examplevm.c
    const uint8_t code[] = "\x30\x60\x00\x52\x59\x60\x00\xf3";
    const size_t code_size = sizeof(code);
    const struct evmc_uint256be code_hash = {.bytes = {1, 2, 3}};
    const uint8_t input[] = "Hello World!";
    const struct evmc_uint256be value = {{1, 0}};
    const struct evmc_address addr = {{0, 1, 2}};
    const int64_t gas = 200000;

    struct evmc_context ctx = {&ctx_fn_table};

    struct evmc_message msg;
    msg.sender = addr;
    msg.destination = addr;
    msg.value = value;
    msg.input_data = input;
    msg.input_size = sizeof(input);
    msg.code_hash = code_hash;
    msg.gas = gas;
    msg.depth = 0;

    struct evmc_result result = evmc_execute(vm, &ctx, EVMC_HOMESTEAD, &msg, code, code_size);

    printf("Execution result:\n");
    if (result.status_code != EVMC_SUCCESS)
    {
        printf("  EVM execution failure: %d\n", result.status_code);
    }
    else
    {
        printf("  Gas used: %" PRId64 "\n", gas - result.gas_left);
        printf("  Gas left: %" PRId64 "\n", result.gas_left);
        printf("  Output size: %zd\n", result.output_size);

        printf("  Output: ");
        size_t i = 0;
        for (i = 0; i < result.output_size; i++)
            printf("%02x ", result.output_data[i]);
        printf("\n");
    }

    evmc_release_result(&result);
    evmc_destroy(vm);

    return 0;
}
