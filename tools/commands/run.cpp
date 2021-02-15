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
int run(evmc::VM& vm,
        evmc_revision rev,
        int64_t gas,
        const std::string& code_hex,
        const std::string& input_hex,
        std::ostream& out)
{
    out << "Executing on " << rev << " with " << gas << " gas limit\n";

    const auto code = from_hex(code_hex);
    const auto input = from_hex(input_hex);
    evmc_message msg{};
    msg.gas = gas;
    msg.input_data = input.data();
    msg.input_size = input.size();
    MockedHost host;

    const auto result = vm.execute(host, rev, msg, code.data(), code.size());

    const auto gas_used = msg.gas - result.gas_left;

    out << "\nResult:   " << result.status_code << "\nGas used: " << gas_used << "\n";

    if (result.status_code == EVMC_SUCCESS || result.status_code == EVMC_REVERT)
        out << "Output:   " << hex(result.output_data, result.output_size) << "\n";

    return 0;
}
}  // namespace cmd
}  // namespace evmc
