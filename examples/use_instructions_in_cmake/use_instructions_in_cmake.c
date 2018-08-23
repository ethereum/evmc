/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */

/** This example shows how to use evmc::instructions library from evmc CMake package. */

#include <evmc/instructions.h>

int main()
{
    return evmc_get_instruction_metrics_table(EVMC_LATEST_REVISION)[OP_STOP]
        .num_stack_returned_items;
}
