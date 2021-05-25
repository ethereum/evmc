// EVMC: Ethereum Client-VM Connector API.
// Copyright 2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "examples/example_vm/example_vm.h"
#include <evmc/tooling.hpp>
#include <gtest/gtest.h>
#include <sstream>

using namespace evmc::tooling;

namespace
{
std::string out_pattern(const char* rev,
                        int gas_limit,
                        const char* status,
                        int gas_used,
                        const char* output = nullptr,
                        bool create = false)
{
    std::ostringstream s;
    s << (create ? "Creating and executing on " : "Executing on ") << rev << " with " << gas_limit
      << " gas limit\n\n"
      << "Result:   " << status << "\nGas used: " << gas_used << "\n";
    if (output != nullptr)
        s << "Output:   " << output << "\n";
    return s.str();
}
}  // namespace

TEST(tool_commands, run_empty_code)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = run(vm, EVMC_FRONTIER, 1, "", "", false, false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(), out_pattern("Frontier", 1, "success", 0, ""));
}

TEST(tool_commands, run_oog)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = run(vm, EVMC_BERLIN, 2, "0x6002600201", "", false, false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(), out_pattern("Berlin", 2, "out of gas", 2));
}

TEST(tool_commands, run_return_my_address)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = run(vm, EVMC_HOMESTEAD, 200, "30600052596000f3", "", false, false, out);
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
        run(vm, EVMC_TANGERINE_WHISTLE, 200, "600035600052596000f3",
            "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f", false, false, out);
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

    const auto exit_code = run(vm, EVMC_SPURIOUS_DRAGON, 200,
                               "6960016000526001601ff3600052600a6016f3", "", true, false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(), out_pattern("Spurious Dragon", 200, "success", 6, "01", true));
}

TEST(tool_commands, create_copy_input_to_output)
{
    // Contract: mstore(0, calldataload(0)) return(0, msize())
    // Create:   mstore(0, 0x600035600052596000f3) return(22, 10)
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code =
        run(vm, EVMC_SPURIOUS_DRAGON, 200, "69600035600052596000f3600052600a6016f3", "0c49c4", true,
            false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(
        out.str(),
        out_pattern("Spurious Dragon", 200, "success", 7,
                    "0c49c40000000000000000000000000000000000000000000000000000000000", true));
}

TEST(tool_commands, create_failure_stack_underflow)
{
    // Contract: n/a
    // Create:   abort()
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = run(vm, EVMC_PETERSBURG, 0, "fe", "", true, false, out);
    EXPECT_EQ(exit_code, EVMC_UNDEFINED_INSTRUCTION);
    EXPECT_EQ(out.str(),
              "Creating and executing on Petersburg with 0 gas limit\n"
              "Contract creation failed: undefined instruction\n");
}

TEST(tool_commands, create_preserve_storage)
{
    // Contract: sload(0) mstore(0) return(31, 1)  6000 54 6000 52 6001 601f f3
    // Create:   sstore(0, 0xbb) mstore(0, "6000546000526001601ff3") return(21, 11)
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code =
        run(vm, EVMC_BERLIN, 200, "60bb 6000 55 6a6000546000526001601ff3 6000 52 600b 6015 f3", "",
            true, false, out);
    EXPECT_EQ(exit_code, 0);
    EXPECT_EQ(out.str(), out_pattern("Berlin", 200, "success", 7, "bb", true));
}

TEST(tool_commands, bench_add)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto exit_code = run(vm, EVMC_LONDON, 200, "6002 80 01", "", false, true, out);
    EXPECT_EQ(exit_code, 0);

    const auto o = out.str();
    EXPECT_NE(o.find("Executing on London"), std::string::npos);
    EXPECT_NE(o.find("Time:     "), std::string::npos);
    EXPECT_NE(o.find("Result:   success"), std::string::npos);
    EXPECT_NE(o.find("Gas used: 3"), std::string::npos);
}

TEST(tool_commands, bench_inconsistent_output)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    std::ostringstream out;

    const auto code = "6000 54 6001 6000 55 6000 52 6001 601f f3";
    const auto exit_code = run(vm, EVMC_BYZANTIUM, 200, code, "", false, true, out);
    EXPECT_EQ(exit_code, 0);

    const auto o = out.str();
    EXPECT_NE(o.find("Executing on Byzantium"), std::string::npos);
    EXPECT_NE(
        o.find(
            "WARNING! Inconsistent execution result likely due to the use of storage (output: 01)"),
        std::string::npos);
    EXPECT_NE(o.find("Time:     "), std::string::npos);
    EXPECT_NE(o.find("Result:   success"), std::string::npos);
    EXPECT_NE(o.find("Gas used: 10"), std::string::npos);
}
