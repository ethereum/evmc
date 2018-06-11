// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include <evmc/instructions.h>

#include <gtest/gtest.h>

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