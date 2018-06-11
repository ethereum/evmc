// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include <evmc/instructions.h>

#include <gtest/gtest.h>

TEST(instructions, homestead_hard_fork)
{
    const auto f = evmc_get_instruction_metrics_table(EVMC_FRONTIER);
    const auto h = evmc_get_instruction_metrics_table(EVMC_HOMESTEAD);

    EXPECT_EQ(f[OP_DELEGATECALL].gas_cost, -1);
    EXPECT_EQ(h[OP_DELEGATECALL].gas_cost, 40);
}

TEST(instructions, tangerine_whistle_hard_fork)
{
    const auto h = evmc_get_instruction_metrics_table(EVMC_HOMESTEAD);
    const auto tw = evmc_get_instruction_metrics_table(EVMC_TANGERINE_WHISTLE);

    EXPECT_EQ(h[OP_EXTCODESIZE].gas_cost, 20);
    EXPECT_EQ(tw[OP_EXTCODESIZE].gas_cost, 700);

    EXPECT_EQ(h[OP_EXTCODECOPY].gas_cost, 20);
    EXPECT_EQ(tw[OP_EXTCODECOPY].gas_cost, 700);

    EXPECT_EQ(h[OP_BALANCE].gas_cost, 20);
    EXPECT_EQ(tw[OP_BALANCE].gas_cost, 400);

    EXPECT_EQ(h[OP_SLOAD].gas_cost, 50);
    EXPECT_EQ(tw[OP_SLOAD].gas_cost, 200);

    EXPECT_EQ(h[OP_CALL].gas_cost, 40);
    EXPECT_EQ(tw[OP_CALL].gas_cost, 700);

    EXPECT_EQ(h[OP_CALLCODE].gas_cost, 40);
    EXPECT_EQ(tw[OP_CALLCODE].gas_cost, 700);

    EXPECT_EQ(h[OP_DELEGATECALL].gas_cost, 40);
    EXPECT_EQ(tw[OP_DELEGATECALL].gas_cost, 700);

    EXPECT_EQ(h[OP_SELFDESTRUCT].gas_cost, 0);
    EXPECT_EQ(tw[OP_SELFDESTRUCT].gas_cost, 5000);
}

TEST(instructions, spurious_dragon_hard_fork)
{
    const auto sd = evmc_get_instruction_metrics_table(EVMC_SPURIOUS_DRAGON);
    const auto tw = evmc_get_instruction_metrics_table(EVMC_TANGERINE_WHISTLE);

    EXPECT_EQ(sd[OP_EXP].gas_cost, 10);
    EXPECT_EQ(tw[OP_EXP].gas_cost, 10);
}

TEST(instructions, byzantium_hard_fork)
{
    const auto b = evmc_get_instruction_metrics_table(EVMC_BYZANTIUM);
    const auto sd = evmc_get_instruction_metrics_table(EVMC_SPURIOUS_DRAGON);

    EXPECT_EQ(b[OP_REVERT].gas_cost, 0);
    EXPECT_EQ(b[OP_REVERT].num_stack_arguments, 2);
    EXPECT_EQ(b[OP_REVERT].num_stack_returned_items, 0);
    EXPECT_EQ(sd[OP_REVERT].gas_cost, -1);

    EXPECT_EQ(b[OP_RETURNDATACOPY].gas_cost, 3);
    EXPECT_EQ(sd[OP_RETURNDATACOPY].gas_cost, -1);

    EXPECT_EQ(b[OP_RETURNDATASIZE].gas_cost, 2);
    EXPECT_EQ(sd[OP_RETURNDATASIZE].gas_cost, -1);

    EXPECT_EQ(b[OP_STATICCALL].gas_cost, 700);
    EXPECT_EQ(sd[OP_STATICCALL].gas_cost, -1);
}