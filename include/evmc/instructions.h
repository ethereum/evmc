/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#pragma once

#include <evmc/evmc.h>

#include <stdint.h>

#if __cplusplus
extern "C" {
#endif

/**
 * Metrics for an EVM 1 instruction.
 *
 * Small integer types are used here to make the tables of metrics cache friendly.
 */
struct evmc_instruction_metrics
{
    /** The instruction gas cost. Value -1 indicates undefined instruction. */
    int16_t gas_cost;

    /** The number of items the instruction pops from the EVM stack before execution. */
    int8_t num_stack_arguments;

    /** The number of items the instruction pushes to the EVM stack after execution. */
    int8_t num_stack_returned_items;
};

/**
 * Get the table of the EVM 1 instructions metrics.
 *
 * @param revision  The EVM revision.
 * @return          The pointer to the array of 256 instruction metrics.
 */
const struct evmc_instruction_metrics* evmc_get_instruction_metrics_table(
    enum evmc_revision revision);

/**
 * Get the table of the EVM 1 instruction names.
 *
 * This table is EVM revision independent and contains the superset of the names of the instructions
 * from all EVM revisions. Use evmc_get_instruction_metrics_table() to know if an instruction
 * is present in the given EVM revision.
 *
 * The entries for undefined instructions contain null pointers.
 *
 * @return  The pointer to the array of 256 instruction names.
 */
const char* const* evmc_get_instruction_name_table();

#if __cplusplus
}
#endif
