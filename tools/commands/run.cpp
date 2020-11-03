// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "tools/commands/commands.hpp"
#include "tools/utils/utils.hpp"
#include <evmc/mocked_host.hpp>
#include <ostream>

namespace evmc
{
namespace cmd
{
namespace
{
constexpr auto deploy_address = 0x000000000000000000000000000000000c9ea7ed_address;
constexpr auto deploy_gas = 10'000'000;
}  // namespace

int run(evmc::VM& vm,
        evmc_revision rev,
        int64_t gas,
        const std::string& code_hex,
        const std::string& input_hex,
        bool create,
        std::ostream& out)
{
    out << "Executing on " << rev << " with " << gas << " gas limit\n";

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
        evmc_message deploy_msg{};
        deploy_msg.kind = EVMC_CREATE;
        deploy_msg.destination = deploy_address;
        deploy_msg.gas = deploy_gas;

        const auto deploy_result = vm.execute(host, rev, deploy_msg, code.data(), code.size());
        if (deploy_result.status_code != EVMC_SUCCESS)
            return 1;

        auto& deployed_account = host.accounts[deploy_address];
        deployed_account.code = bytes(deploy_result.output_data, deploy_result.output_size);

        msg.destination = deploy_address;

        exec_code_data = deployed_account.code.data();
        exec_code_size = deployed_account.code.size();
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
