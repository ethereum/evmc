/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */

#include <evmc/evmc.h>
#include <evmc/helpers.h>

#include <inttypes.h>
#include <stdio.h>

#include "examplevm/examplevm.h"

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
