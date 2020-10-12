// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "tools/commands/commands.hpp"
#include <CLI/CLI.hpp>
#include <evmc/loader.h>

int main(int argc, const char** argv)
{
    using namespace evmc;

    CLI::App app{"EVMC tool"};
    const auto& version_flag = *app.add_flag("--version", "Print version information");

    std::string vm_config;
    std::string code_hex;
    int64_t gas = 1000000;
    auto rev = EVMC_ISTANBUL;
    std::string input_hex;

    auto& run_cmd = *app.add_subcommand("run", "Execute EVM bytecode");
    run_cmd.add_option("code", code_hex, "Hex-encoded bytecode")->required();
    run_cmd.add_option("--vm", vm_config, "EVMC VM module")->required()->envname("EVMC_VM");
    run_cmd.add_option("--gas", gas, "Execution gas limit", true)->check(CLI::Range(0, 1000000000));
    run_cmd.add_option("--rev", rev, "EVM revision", true);
    run_cmd.add_option("--input", input_hex, "Hex-encoded input bytes");

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
            evmc_loader_error_code ec;
            auto vm = VM{evmc_load_and_configure(vm_config.c_str(), &ec)};
            if (ec != EVMC_LOADER_SUCCESS)
            {
                const auto error = evmc_last_error_msg();
                if (error != nullptr)
                    std::cerr << error << "\n";
                else
                    std::cerr << "Loading error " << ec << "\n";
                return static_cast<int>(ec);
            }
            std::cout << "Config: " << vm_config << "\n";
            return cmd::run(vm, rev, gas, code_hex, input_hex, std::cout);
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
