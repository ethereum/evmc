// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <CLI/CLI.hpp>
#include <evmc/loader.h>
#include <evmc/mocked_host.hpp>

#include "utils.hpp"

namespace evmc
{
int check_command(VM& vm);
}

int main(int argc, const char** argv)
{
    using namespace evmc;

    CLI::App app{"EVMC tool"};
    const auto& version_flag = *app.add_flag("--version", "Print version information");

    std::string vm_config;
    evmc::VM vm;
    std::function<void(const std::string&)> load_vm = [&vm, &vm_config](const std::string& config) {
        evmc_loader_error_code ec;
        vm = VM{evmc_load_and_configure(config.c_str(), &ec)};
        if (ec != EVMC_LOADER_SUCCESS)
        {
            const auto error = evmc_last_error_msg();
            throw std::invalid_argument{error != nullptr ? error : "Loading error"};
        }
        vm_config = config;
    };

    std::string code_hex;
    evmc_message msg{};
    msg.gas = 1000000;
    auto rev = EVMC_ISTANBUL;

    auto& run_cmd = *app.add_subcommand("run", "Execute EVM bytecode");
    run_cmd.add_option_function("--vm", load_vm, "EVMC VM module")->required()->envname("EVMC_VM");

    run_cmd.add_option("code", code_hex, "Hex-encoded bytecode")->required();
    run_cmd.add_option("--gas", msg.gas, "Execution gas limit", true)
        ->check(CLI::Range(0, 1000000000));
    run_cmd.add_option("--rev", rev, "EVM revision", true);

    auto& check_cmd = *app.add_subcommand("check", "Check VM for compatibility with EVMC");
    check_cmd.add_option_function("vm", load_vm, "EVMC VM module")->required();

    try
    {
        app.parse(argc, argv);

        // Handle the --version flag first and exit when present.
        if (version_flag)
        {
            std::cout << "EVMC tool " PROJECT_VERSION "\n";
            return 0;
        }

        if (run_cmd)
        {
            const auto code = from_hex(code_hex);

            MockedHost host;

            std::cout << "Executing on " << rev << " with " << msg.gas << " gas limit\n"
                      << "in " << vm_config << "\n";
            const auto result = vm.execute(host, rev, msg, code.data(), code.size());

            const auto gas_used = msg.gas - result.gas_left;

            std::cout << "\nResult:   " << result.status_code << "\nGas used: " << gas_used << "\n";

            if (result.status_code == EVMC_SUCCESS || result.status_code == EVMC_REVERT)
                std::cout << "Output:   " << hex(result.output_data, result.output_size) << "\n";

            return 0;
        }
        else if (check_cmd)
            check_command(vm);

        return 0;
    }
    catch (const CLI::ParseError& e)
    {
        return app.exit(e);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }
    catch (...)
    {
        return -2;
    }
}
