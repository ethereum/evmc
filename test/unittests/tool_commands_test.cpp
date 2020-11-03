// EVMC: Ethereum Client-VM Connector API.
// Copyright 2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "examples/example_vm/example_vm.h"
#include "tools/commands/commands.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace evmc;

namespace
{
std::string out_pattern(const char* rev,
                        int gas_limit,
                        const char* status,
                        int gas_used,
                        const char* output = nullptr)
{
    std::ostringstream s;
    s << "Executing on " << rev << " with " << gas_limit << " gas limit\n\n"
      << "Result:   " << status << "\nGas used: " << gas_used << "\n";
    if (output)
        s << "Output:   " << output << "\n";
    return s.str();
}
}  // namespace

TEST(tool_commands, run_empty_code)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = cmd::run(vm, EVMC_FRONTIER, 1, "", "", false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(), out_pattern("Frontier", 1, "success", 0, ""));
}

TEST(tool_commands, run_return_my_address)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = cmd::run(vm, EVMC_HOMESTEAD, 200, "30600052596000f3", "", false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(),
              out_pattern("Homestead", 200, "success", 6,
                          "0000000000000000000000000000000000000000000000000000000000000000"));
}

TEST(tool_commands, run_copy_input_to_output)
{
    // Yul: mstore(0, calldataload(0)) return(0, msize())
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code =
        cmd::run(vm, EVMC_TANGERINE_WHISTLE, 200, "600035600052596000f3",
                 "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f", false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(),
              out_pattern("Tangerine Whistle", 200, "success", 7,
                          "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"));
}

TEST(tool_commands, create_return_1)
{
    // Contract: mstore(0, 1) return(31, 1)
    // Create:   mstore(0, 0x60016000526001601ff3) return(22, 10)
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = cmd::run(vm, EVMC_SPURIOUS_DRAGON, 200,
                                    "6960016000526001601ff3600052600a6016f3", "", true, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(), out_pattern("Spurious Dragon", 200, "success", 6, "01"));
}
