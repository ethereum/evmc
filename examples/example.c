/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2016-2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#include "example_host.h"
#ifdef STATICALLY_LINKED_EXAMPLE
#include "example_vm/example_vm.h"
#endif

#include <evmc/helpers.h>
#include <evmc/loader.h>

#include <inttypes.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
#ifdef STATICALLY_LINKED_EXAMPLE
    (void)argc;
    (void)argv;
    struct evmc_vm* vm = evmc_create_example_vm();
    if (!vm)
        return EVMC_LOADER_VM_CREATION_FAILURE;
    if (!evmc_is_abi_compatible(vm))
        return EVMC_LOADER_ABI_VERSION_MISMATCH;
#else
    const char* config_string = (argc > 1) ? argv[1] : "example-vm.so";
    enum evmc_loader_error_code error_code;
    struct evmc_vm* vm = evmc_load_and_configure(config_string, &error_code);
    if (!vm)
    {
        printf("Loading error: %d\n", error_code);
        // NOTE: the values are small enough for casting
        return (int)error_code;
    }
#endif

    // EVM bytecode goes here. This is one of the examples.
    const uint8_t code[] = "\x43\x60\x00\x55\x43\x60\x00\x52\x59\x60\x00\xf3";
    const size_t code_size = sizeof(code) - 1;
    const uint8_t input[] = "Hello World!";
    const evmc_uint256be value = {{1, 0}};
    const evmc_address addr = {{0, 1, 2}};
    const int64_t gas = 200000;
    struct evmc_tx_context tx_context;
    memset(&tx_context, 0, sizeof(tx_context));
    tx_context.block_number = 42;
    tx_context.block_timestamp = 66;
    tx_context.block_gas_limit = gas * 2;
    const struct evmc_host_interface* host = example_host_get_interface();
    struct evmc_host_context* ctx = example_host_create_context(tx_context);
    struct evmc_message msg;
    msg.kind = EVMC_CALL;
    msg.sender = addr;
    msg.destination = addr;
    msg.value = value;
    msg.input_data = input;
    msg.input_size = sizeof(input);
    msg.gas = gas;
    msg.depth = 0;
    struct evmc_result result = evmc_execute(vm, host, ctx, EVMC_HOMESTEAD, &msg, code, code_size);
    printf("Execution result:\n");
    int exit_code = 0;
    if (result.status_code != EVMC_SUCCESS)
    {
        printf("  EVM execution failure: %d\n", result.status_code);
        exit_code = result.status_code;
    }
    else
    {
        printf("  Gas used: %" PRId64 "\n", gas - result.gas_left);
        printf("  Gas left: %" PRId64 "\n", result.gas_left);
        printf("  Output size: %zd\n", result.output_size);
        printf("  Output: ");
        size_t i = 0;
        for (i = 0; i < result.output_size; i++)
            printf("%02x", result.output_data[i]);
        printf("\n");
        const evmc_bytes32 storage_key = {{0}};
        evmc_bytes32 storage_value = host->get_storage(ctx, &msg.destination, &storage_key);
        printf("  Storage at 0x00..00: ");
        for (i = 0; i < sizeof(storage_value.bytes) / sizeof(storage_value.bytes[0]); i++)
            printf("%02x", storage_value.bytes[i]);
        printf("\n");
    }
    evmc_release_result(&result);
    example_host_destroy_context(ctx);
    evmc_destroy(vm);
    return exit_code;
}
