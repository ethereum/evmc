// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "tools/commands/commands.hpp"
#include "tools/utils/utils.hpp"
#include <evmc/hex.hpp>
#include <evmc/mocked_host.hpp>
#include <ostream>

namespace evmc
{
namespace cmd
{
namespace
{
/// The address where a new contract is created with --create option.
constexpr auto create_address = 0xc9ea7ed000000000000000000000000000000001_address;

/// The gas limit for contract creation.
constexpr auto create_gas = 10'000'000;
}  // namespace

int run(evmc::VM& vm,
        evmc_revision rev,
        int64_t gas,
        const std::string& code_hex,
        const std::string& input_hex,
        bool create,
        std::ostream& out)
{
    out << (create ? "Creating and executing on " : "Executing on ") << rev << " with " << gas
        << " gas limit\n";

    const auto code = from_hex(code_hex);
    const auto input = from_hex(input_hex);

    MockedHost host;

    evmc_message msg{};
    msg.gas = gas;
    msg.input_data = input.data();
    msg.input_size = input.size();

    const uint8_t* exec_code_data = nullptr;
    size_t exec_code_size = 0;

    if (create)
    {
        evmc_message create_msg{};
        create_msg.kind = EVMC_CREATE;
        create_msg.destination = create_address;
        create_msg.gas = create_gas;

        const auto create_result = vm.execute(host, rev, create_msg, code.data(), code.size());
        if (create_result.status_code != EVMC_SUCCESS)
        {
            out << "Contract creation failed: " << create_result.status_code << "\n";
            return create_result.status_code;
        }

        auto& created_account = host.accounts[create_address];
        created_account.code = bytes(create_result.output_data, create_result.output_size);

        msg.destination = create_address;

        exec_code_data = created_account.code.data();
        exec_code_size = created_account.code.size();
    }
    else
    {
        exec_code_data = code.data();
        exec_code_size = code.size();
    }

    const auto result = vm.execute(host, rev, msg, exec_code_data, exec_code_size);

    const auto gas_used = msg.gas - result.gas_left;

    out << "\nResult:   " << result.status_code << "\nGas used: " << gas_used << "\n";

    if (result.status_code == EVMC_SUCCESS || result.status_code == EVMC_REVERT)
        out << "Output:   " << hex(result.output_data, result.output_size) << "\n";

    return 0;
}
}  // namespace cmd
}  // namespace evmc
