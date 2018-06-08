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
 * The list of EVM 1 instructions from every EVM revision.
 */
enum evmc_instruction
{
    STOP = 0x00,
    ADD,
    MUL,
    SUB,
    DIV,
    SDIV,
    MOD,
    SMOD,
    ADDMOD,
    MULMOD,
    EXP,
    SIGNEXTEND,

    LT = 0x10,
    GT,
    SLT,
    SGT,
    EQ,
    ISZERO,
    AND,
    OR,
    XOR,
    NOT,
    BYTE,
    SHL,
    SHR,
    SAR,

    SHA3 = 0x20,

    ADDRESS = 0x30,
    BALANCE,
    ORIGIN,
    CALLER,
    CALLVALUE,
    CALLDATALOAD,
    CALLDATASIZE,
    CALLDATACOPY,
    CODESIZE,
    CODECOPY,
    GASPRICE,
    EXTCODESIZE,
    EXTCODECOPY,
    RETURNDATASIZE = 0x3d,
    RETURNDATACOPY = 0x3e,

    BLOCKHASH = 0x40,
    COINBASE,
    TIMESTAMP,
    NUMBER,
    DIFFICULTY,
    GASLIMIT,

    POP = 0x50,
    MLOAD,
    MSTORE,
    MSTORE8,
    SLOAD,
    SSTORE,
    JUMP,
    JUMPI,
    PC,
    MSIZE,
    GAS,
    JUMPDEST,

    PUSH1 = 0x60,
    PUSH2,
    PUSH3,
    PUSH4,
    PUSH5,
    PUSH6,
    PUSH7,
    PUSH8,
    PUSH9,
    PUSH10,
    PUSH11,
    PUSH12,
    PUSH13,
    PUSH14,
    PUSH15,
    PUSH16,
    PUSH17,
    PUSH18,
    PUSH19,
    PUSH20,
    PUSH21,
    PUSH22,
    PUSH23,
    PUSH24,
    PUSH25,
    PUSH26,
    PUSH27,
    PUSH28,
    PUSH29,
    PUSH30,
    PUSH31,
    PUSH32,

    DUP1 = 0x80,
    DUP2,
    DUP3,
    DUP4,
    DUP5,
    DUP6,
    DUP7,
    DUP8,
    DUP9,
    DUP10,
    DUP11,
    DUP12,
    DUP13,
    DUP14,
    DUP15,
    DUP16,

    SWAP1 = 0x90,
    SWAP2,
    SWAP3,
    SWAP4,
    SWAP5,
    SWAP6,
    SWAP7,
    SWAP8,
    SWAP9,
    SWAP10,
    SWAP11,
    SWAP12,
    SWAP13,
    SWAP14,
    SWAP15,
    SWAP16,

    LOG0 = 0xa0,
    LOG1,
    LOG2,
    LOG3,
    LOG4,

    CREATE = 0xf0,
    CALL,
    CALLCODE,
    RETURN,
    DELEGATECALL,
    STATICCALL = 0xfa,

    REVERT = 0xfd,
    INVALID = 0xfe,
    SELFDESTRUCT = 0xff,
};

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
