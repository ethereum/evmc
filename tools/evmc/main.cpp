// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "tools/commands/commands.hpp"
#include <CLI/CLI.hpp>
#include <evmc/loader.h>
#include <fstream>

bool is_file_readable(const std::string& path)
{
    std::cerr << "Trying " << path;
    std::ifstream f{path, std::ios::in | std::ios::binary};
    const auto result = f.is_open();
    std::cerr << ": " << result << "\n";
    return result;
}

int main(int argc, const char** argv)
{
    std::string arg0{argv[0]};
    std::cerr << "argv[0]: " << arg0 << "\n";

    std::string dirname = "./";

    constexpr auto separator = '/';
    auto pos = arg0.rfind(separator);
    if (pos == std::string::npos)
        pos = 0;
    else
    {
        ++pos;
        dirname = arg0.substr(0, pos);
    }
    const auto tool_name = arg0.substr(pos);
    std::cerr << "tool name: " << tool_name << "\n";
    std::cerr << "dir name:  " << dirname << "\n";

    bool special_mode = tool_name != "evmc";

    std::string vm_config;
    if (special_mode)
    {
        auto p = dirname + "lib" + tool_name + ".so";
        if (is_file_readable(p))
            vm_config = p;
        else if (is_file_readable(p = dirname + "../lib/" + "lib" + tool_name + ".so"))
            vm_config = p;
    }

    using namespace evmc;

    CLI::App app{"EVMC tool"};
    const auto& version_flag = *app.add_flag("--version", "Print version information");

    std::string code_hex;
    int64_t gas = 1000000;
    auto rev = EVMC_ISTANBUL;
    std::string input_hex;
    auto create = false;

    auto& run_cmd = *app.add_subcommand("run", "Execute EVM bytecode");
    run_cmd.add_option("code", code_hex, "Hex-encoded bytecode")->required();

    if (!special_mode)
        run_cmd.add_option("--vm", vm_config, "EVMC VM module")->required()->envname("EVMC_VM");

    run_cmd.add_option("--gas", gas, "Execution gas limit", true)->check(CLI::Range(0, 1000000000));
    run_cmd.add_option("--rev", rev, "EVM revision", true);
    run_cmd.add_option("--input", input_hex, "Hex-encoded input bytes");
    run_cmd.add_option(
        "--create", create,
        "Create new contract out of the code and then execute this contract with the input");

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
