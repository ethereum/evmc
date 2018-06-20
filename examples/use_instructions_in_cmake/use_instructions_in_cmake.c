/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

/** This example shows how to use evmc::instructions library from evmc CMake package. */

#include <evmc/instructions.h>

int main()
{
    return evmc_get_instruction_metrics_table(EVMC_LATEST_REVISION)[OP_STOP]
        .num_stack_returned_items;
}
