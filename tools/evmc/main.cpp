// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "tools/commands/commands.hpp"
#include <CLI/CLI.hpp>
#include <evmc/loader.h>

int main(int argc, const char** argv)
{
    using namespace evmc;

    std::string vm_config;
    std::string code_hex;
    int64_t gas = 1000000;
    auto rev = EVMC_ISTANBUL;
    std::string input_hex;
    auto create = false;

    CLI::App app{"EVMC tool"};
    const auto& version_flag = *app.add_flag("--version", "Print version information and exit");
    const auto& vm_option =
        *app.add_option("--vm", vm_config, "EVMC VM module")->envname("EVMC_VM");

    auto& run_cmd = *app.add_subcommand("run", "Execute EVM bytecode");
    run_cmd.add_option("code", code_hex, "Hex-encoded bytecode")->required();
    run_cmd.add_option("--gas", gas, "Execution gas limit", true)->check(CLI::Range(0, 1000000000));
    run_cmd.add_option("--rev", rev, "EVM revision", true);
    run_cmd.add_option("--input", input_hex, "Hex-encoded input bytes");
    run_cmd.add_flag(
        "--create", create,
        "Create new contract out of the code and then execute this contract with the input");

    try
    {
        app.parse(argc, argv);

        evmc::VM vm;
        if (vm_option.count() != 0)
        {
            evmc_loader_error_code ec;
            vm = VM{evmc_load_and_configure(vm_config.c_str(), &ec)};
            if (ec != EVMC_LOADER_SUCCESS)
            {
                const auto error = evmc_last_error_msg();
                if (error != nullptr)
                    std::cerr << error << "\n";
                else
                    std::cerr << "Loading error " << ec << "\n";
                return static_cast<int>(ec);
            }
        }

        // Handle the --version flag first and exit when present.
        if (version_flag)
        {
            if (vm)
                std::cout << vm.name() << " " << vm.version() << " (" << vm_config << ")\n";

            std::cout << "EVMC " PROJECT_VERSION;
            if (argc >= 1)
                std::cout << " (" << argv[0] << ")";
            std::cout << "\n";
            return 0;
        }

        if (run_cmd)
        {
            // For run command the --vm is required.
            if (vm_option.count() == 0)
                throw CLI::RequiredError{vm_option.get_name()};

            std::cout << "Config: " << vm_config << "\n";
            return cmd::run(vm, rev, gas, code_hex, input_hex, create, std::cout);
        }

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
