// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/instructions.h>

#include <gtest/gtest.h>

bool operator==(const evmc_instruction_metrics& a, const evmc_instruction_metrics& b) noexcept
{
    return a.gas_cost == b.gas_cost && a.num_stack_arguments == b.num_stack_arguments &&
           a.num_stack_returned_items == b.num_stack_returned_items;
}

TEST(instructions, name_gas_cost_equivalence)
{
    for (auto r = int{EVMC_FRONTIER}; r <= EVMC_MAX_REVISION; ++r)
    {
        const auto rev = static_cast<evmc_revision>(r);
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

TEST(instructions, homestead_hard_fork)
{
    const auto f = evmc_get_instruction_metrics_table(EVMC_FRONTIER);
    const auto h = evmc_get_instruction_metrics_table(EVMC_HOMESTEAD);
    const auto fn = evmc_get_instruction_names_table(EVMC_FRONTIER);
    const auto hn = evmc_get_instruction_names_table(EVMC_HOMESTEAD);

    for (int op{OP_STOP}; op <= OP_SELFDESTRUCT; ++op)
    {
        switch (op)  // NOLINT
        {
        case OP_DELEGATECALL:
            continue;
        default:
            EXPECT_EQ(h[op], f[op]) << op;
            EXPECT_STREQ(hn[op], fn[op]) << op;
            break;
        }
    }

    EXPECT_EQ(f[OP_DELEGATECALL].gas_cost, -1);
    EXPECT_EQ(h[OP_DELEGATECALL].gas_cost, 40);
    EXPECT_TRUE(fn[OP_DELEGATECALL] == nullptr);
    EXPECT_EQ(hn[OP_DELEGATECALL], std::string{"DELEGATECALL"});
}

TEST(instructions, tangerine_whistle_hard_fork)
{
    const auto h = evmc_get_instruction_metrics_table(EVMC_HOMESTEAD);
    const auto tw = evmc_get_instruction_metrics_table(EVMC_TANGERINE_WHISTLE);
    const auto hn = evmc_get_instruction_names_table(EVMC_HOMESTEAD);
    const auto twn = evmc_get_instruction_names_table(EVMC_TANGERINE_WHISTLE);

    for (int op{OP_STOP}; op <= OP_SELFDESTRUCT; ++op)
    {
        switch (op)
        {
        case OP_EXTCODESIZE:
        case OP_EXTCODECOPY:
        case OP_BALANCE:
        case OP_SLOAD:
        case OP_CALL:
        case OP_CALLCODE:
        case OP_DELEGATECALL:
        case OP_SELFDESTRUCT:
            continue;
        default:
            EXPECT_EQ(tw[op], h[op]) << op;
            EXPECT_STREQ(twn[op], hn[op]) << op;
            break;
        }
    }

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
    const auto sdn = evmc_get_instruction_names_table(EVMC_SPURIOUS_DRAGON);
    const auto twn = evmc_get_instruction_names_table(EVMC_TANGERINE_WHISTLE);

    for (int op{OP_STOP}; op <= OP_SELFDESTRUCT; ++op)
    {
        switch (op)  // NOLINT
        {
        case OP_EXP:
            continue;
        default:
            EXPECT_EQ(sd[op], tw[op]) << op;
            EXPECT_STREQ(sdn[op], twn[op]) << op;
            break;
        }
    }

    EXPECT_EQ(sd[OP_EXP].gas_cost, 10);
    EXPECT_EQ(tw[OP_EXP].gas_cost, 10);
}

TEST(instructions, byzantium_hard_fork)
{
    const auto b = evmc_get_instruction_metrics_table(EVMC_BYZANTIUM);
    const auto sd = evmc_get_instruction_metrics_table(EVMC_SPURIOUS_DRAGON);
    const auto bn = evmc_get_instruction_names_table(EVMC_BYZANTIUM);
    const auto sdn = evmc_get_instruction_names_table(EVMC_SPURIOUS_DRAGON);

    for (int op{OP_STOP}; op <= OP_SELFDESTRUCT; ++op)
    {
        switch (op)
        {
        case OP_REVERT:
        case OP_RETURNDATACOPY:
        case OP_RETURNDATASIZE:
        case OP_STATICCALL:
            continue;
        default:
            EXPECT_EQ(b[op], sd[op]) << op;
            EXPECT_STREQ(bn[op], sdn[op]) << op;
            break;
        }
    }

    EXPECT_EQ(b[OP_REVERT].gas_cost, 0);
    EXPECT_EQ(b[OP_REVERT].num_stack_arguments, 2);
    EXPECT_EQ(b[OP_REVERT].num_stack_returned_items, 0);
    EXPECT_EQ(sd[OP_REVERT].gas_cost, -1);
    EXPECT_EQ(bn[OP_REVERT], std::string{"REVERT"});
    EXPECT_TRUE(sdn[OP_REVERT] == nullptr);

    EXPECT_EQ(b[OP_RETURNDATACOPY].gas_cost, 3);
    EXPECT_EQ(sd[OP_RETURNDATACOPY].gas_cost, -1);
    EXPECT_EQ(bn[OP_RETURNDATACOPY], std::string{"RETURNDATACOPY"});
    EXPECT_TRUE(sdn[OP_RETURNDATACOPY] == nullptr);

    EXPECT_EQ(b[OP_RETURNDATASIZE].gas_cost, 2);
    EXPECT_EQ(sd[OP_RETURNDATASIZE].gas_cost, -1);
    EXPECT_EQ(bn[OP_RETURNDATASIZE], std::string{"RETURNDATASIZE"});
    EXPECT_TRUE(sdn[OP_RETURNDATASIZE] == nullptr);

    EXPECT_EQ(b[OP_STATICCALL].gas_cost, 700);
    EXPECT_EQ(sd[OP_STATICCALL].gas_cost, -1);
    EXPECT_EQ(bn[OP_STATICCALL], std::string{"STATICCALL"});
    EXPECT_TRUE(sdn[OP_STATICCALL] == nullptr);
}

TEST(instructions, constantinople_hard_fork)
{
    const auto c = evmc_get_instruction_metrics_table(EVMC_CONSTANTINOPLE);
    const auto b = evmc_get_instruction_metrics_table(EVMC_BYZANTIUM);
    const auto cn = evmc_get_instruction_names_table(EVMC_CONSTANTINOPLE);
    const auto bn = evmc_get_instruction_names_table(EVMC_BYZANTIUM);

    for (int op{OP_STOP}; op <= OP_SELFDESTRUCT; ++op)
    {
        switch (op)
        {
        case OP_CREATE2:
        case OP_EXTCODEHASH:
        case OP_SHL:
        case OP_SHR:
        case OP_SAR:
            continue;
        default:
            EXPECT_EQ(c[op], b[op]) << op;
            EXPECT_STREQ(cn[op], bn[op]) << op;
            break;
        }
    }

    for (auto op : {OP_SHL, OP_SHR, OP_SAR})
    {
        const auto m = c[op];
        EXPECT_EQ(m.gas_cost, 3);
        EXPECT_EQ(m.num_stack_arguments, 2);
        EXPECT_EQ(m.num_stack_returned_items, 1);
    }

    EXPECT_EQ(c[OP_CREATE2].gas_cost, 32000);
    EXPECT_EQ(c[OP_CREATE2].num_stack_arguments, 4);
    EXPECT_EQ(c[OP_CREATE2].num_stack_returned_items, 1);
    EXPECT_EQ(b[OP_CREATE2].gas_cost, -1);
    EXPECT_EQ(cn[OP_CREATE2], std::string{"CREATE2"});
    EXPECT_TRUE(bn[OP_CREATE2] == nullptr);

    EXPECT_EQ(c[OP_EXTCODEHASH].gas_cost, 400);
    EXPECT_EQ(c[OP_EXTCODEHASH].num_stack_arguments, 1);
    EXPECT_EQ(c[OP_EXTCODEHASH].num_stack_returned_items, 1);
    EXPECT_EQ(b[OP_EXTCODEHASH].gas_cost, -1);
    EXPECT_EQ(cn[OP_EXTCODEHASH], std::string{"EXTCODEHASH"});
    EXPECT_TRUE(bn[OP_EXTCODEHASH] == nullptr);
}

TEST(instructions, petersburg_hard_fork)
{
    const auto p = evmc_get_instruction_metrics_table(EVMC_PETERSBURG);
    const auto c = evmc_get_instruction_metrics_table(EVMC_CONSTANTINOPLE);
    const auto pn = evmc_get_instruction_names_table(EVMC_PETERSBURG);
    const auto cn = evmc_get_instruction_names_table(EVMC_CONSTANTINOPLE);

    for (int op{OP_STOP}; op <= OP_SELFDESTRUCT; ++op)
    {
        EXPECT_EQ(p[op], c[op]) << op;
        EXPECT_STREQ(pn[op], cn[op]) << op;
    }
}

TEST(instructions, istanbul_hard_fork)
{
    const auto i = evmc_get_instruction_metrics_table(EVMC_ISTANBUL);
    const auto p = evmc_get_instruction_metrics_table(EVMC_PETERSBURG);
    const auto in = evmc_get_instruction_names_table(EVMC_ISTANBUL);
    const auto pn = evmc_get_instruction_names_table(EVMC_PETERSBURG);

    for (int op{OP_STOP}; op <= OP_SELFDESTRUCT; ++op)
    {
        EXPECT_EQ(i[op], p[op]) << op;
        EXPECT_STREQ(in[op], pn[op]) << op;
    }
}
