// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include <evmc/instructions.h>

#include <gtest/gtest.h>

TEST(instructions, tangerine_whistle_hard_fork)
{
    const auto h = evmc_get_instruction_metrics_table(EVMC_HOMESTEAD);
    const auto tw = evmc_get_instruction_metrics_table(EVMC_TANGERINE_WHISTLE);

    EXPECT_EQ(h[EXTCODESIZE].gas_cost, 20);
    EXPECT_EQ(tw[EXTCODESIZE].gas_cost, 700);

    EXPECT_EQ(h[EXTCODECOPY].gas_cost, 20);
    EXPECT_EQ(tw[EXTCODECOPY].gas_cost, 700);

    EXPECT_EQ(h[BALANCE].gas_cost, 20);
    EXPECT_EQ(tw[BALANCE].gas_cost, 400);

    EXPECT_EQ(h[SLOAD].gas_cost, 50);
    EXPECT_EQ(tw[SLOAD].gas_cost, 200);

    EXPECT_EQ(h[CALL].gas_cost, 40);
    EXPECT_EQ(tw[CALL].gas_cost, 700);

    EXPECT_EQ(h[CALLCODE].gas_cost, 40);
    EXPECT_EQ(tw[CALLCODE].gas_cost, 700);

    EXPECT_EQ(h[DELEGATECALL].gas_cost, 40);
    EXPECT_EQ(tw[DELEGATECALL].gas_cost, 700);

    EXPECT_EQ(h[SELFDESTRUCT].gas_cost, 0);
    EXPECT_EQ(tw[SELFDESTRUCT].gas_cost, 5000);
}