// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#include <evmc/instructions.h>

#include <gtest/gtest.h>

TEST(instructions, homestead_hard_fork)
{
    const auto f = evmc_get_instruction_metrics_table(EVMC_FRONTIER);
    const auto h = evmc_get_instruction_metrics_table(EVMC_HOMESTEAD);
    const auto fn = evmc_get_instruction_names_table(EVMC_FRONTIER);
    const auto hn = evmc_get_instruction_names_table(EVMC_HOMESTEAD);

    EXPECT_EQ(f[OP_DELEGATECALL].gas_cost, -1);
    EXPECT_EQ(h[OP_DELEGATECALL].gas_cost, 40);
    EXPECT_EQ(fn[OP_DELEGATECALL], nullptr);
    EXPECT_EQ(hn[OP_DELEGATECALL], std::string{"DELEGATECALL"});
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
    const auto bn = evmc_get_instruction_names_table(EVMC_BYZANTIUM);
    const auto sdn = evmc_get_instruction_names_table(EVMC_SPURIOUS_DRAGON);

    EXPECT_EQ(b[OP_REVERT].gas_cost, 0);
    EXPECT_EQ(b[OP_REVERT].num_stack_arguments, 2);
    EXPECT_EQ(b[OP_REVERT].num_stack_returned_items, 0);
    EXPECT_EQ(sd[OP_REVERT].gas_cost, -1);
    EXPECT_EQ(bn[OP_REVERT], std::string{"REVERT"});
    EXPECT_EQ(sdn[OP_REVERT], nullptr);

    EXPECT_EQ(b[OP_RETURNDATACOPY].gas_cost, 3);
    EXPECT_EQ(sd[OP_RETURNDATACOPY].gas_cost, -1);
    EXPECT_EQ(bn[OP_RETURNDATACOPY], std::string{"RETURNDATACOPY"});
    EXPECT_EQ(sdn[OP_RETURNDATACOPY], nullptr);

    EXPECT_EQ(b[OP_RETURNDATASIZE].gas_cost, 2);
    EXPECT_EQ(sd[OP_RETURNDATASIZE].gas_cost, -1);
    EXPECT_EQ(bn[OP_RETURNDATASIZE], std::string{"RETURNDATASIZE"});
    EXPECT_EQ(sdn[OP_RETURNDATASIZE], nullptr);

    EXPECT_EQ(b[OP_STATICCALL].gas_cost, 700);
    EXPECT_EQ(sd[OP_STATICCALL].gas_cost, -1);
    EXPECT_EQ(bn[OP_STATICCALL], std::string{"STATICCALL"});
    EXPECT_EQ(sdn[OP_STATICCALL], nullptr);
}

TEST(instructions, constantinople_hard_fork)
{
    const auto c = evmc_get_instruction_metrics_table(EVMC_CONSTANTINOPLE);
    const auto b = evmc_get_instruction_metrics_table(EVMC_BYZANTIUM);
    const auto cn = evmc_get_instruction_names_table(EVMC_CONSTANTINOPLE);
    const auto bn = evmc_get_instruction_names_table(EVMC_BYZANTIUM);

    EXPECT_EQ(c[OP_CREATE2].gas_cost, 32000);
    EXPECT_EQ(c[OP_CREATE2].num_stack_arguments, 4);
    EXPECT_EQ(c[OP_CREATE2].num_stack_returned_items, 1);
    EXPECT_EQ(b[OP_CREATE2].gas_cost, -1);
    EXPECT_EQ(cn[OP_CREATE2], std::string{"CREATE2"});
    EXPECT_EQ(bn[OP_CREATE2], nullptr);

    EXPECT_EQ(c[OP_EXTCODEHASH].gas_cost, 400);
    EXPECT_EQ(c[OP_EXTCODEHASH].num_stack_arguments, 1);
    EXPECT_EQ(c[OP_EXTCODEHASH].num_stack_returned_items, 1);
    EXPECT_EQ(b[OP_EXTCODEHASH].gas_cost, -1);
    EXPECT_EQ(cn[OP_EXTCODEHASH], std::string{"EXTCODEHASH"});
    EXPECT_EQ(bn[OP_EXTCODEHASH], nullptr);
}


TEST(instructions, name_gas_cost_equivalence)
{
    for (auto rev = EVMC_FRONTIER; rev <= EVMC_MAX_REVISION;
         rev = static_cast<evmc_revision>(rev + 1))
    {
        const auto names = evmc_get_instruction_names_table(rev);
        const auto metrics = evmc_get_instruction_metrics_table(rev);

        for (int i = 0; i < 256; ++i)
        {
            auto name = names[i];
            auto gas_cost = metrics[i].gas_cost;

            if (name != nullptr)
                EXPECT_GE(gas_cost, 0);
            else
                EXPECT_EQ(gas_cost, -1);
        }
    }
}
