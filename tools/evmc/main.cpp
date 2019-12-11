// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/loader.h>
#include <evmc/mocked_host.hpp>
#include <iterator>
#include <sstream>

#include <iostream>

#include "utils.hpp"

namespace CLI
{
std::istringstream& operator>>(std::istringstream& in, evmc::bytes& val)
{
    using iterator_type = std::istream_iterator<std::istringstream::char_type>;
    std::copy(iterator_type{in}, iterator_type{}, std::back_inserter(val));
    in.clear(std::ios::eofbit);  // Clear fail state.
    return in;
}
}  // namespace CLI

#include <CLI/CLI.hpp>

namespace CLI
{
namespace detail
{
template <>
constexpr const char* type_name<evmc::bytes>()
{
    return "BYTES";
}
}  // namespace detail

class Bytes : public CLI::Validator
{
public:
    Bytes() : Validator{"HEX|FILE"}
    {
        non_modifying_ = false;
        func_ = [](std::string& input) -> std::string {
            std::cerr << "in: " << input << "\n";
            input = evmc::from_hex(input);
            std::cerr << "out: " << input << "\n";
            return {};
        };
    }
};
}  // namespace CLI

int main(int argc, const char** argv)
{
    using namespace evmc;

    CLI::App app{"EVMC tool"};
    const auto& version_flag = *app.add_flag("--version", "Print version information");

    std::string vm_config;
    bytes code;
    evmc_message msg{};
    msg.gas = 1000000;
    auto rev = EVMC_ISTANBUL;

    auto& run_cmd = *app.add_subcommand("run", "Execute EVM bytecode");
    run_cmd.add_option("code", code, "Bytecode")->required()->check(CLI::Bytes{});
    run_cmd.add_option("--vm", vm_config, "EVMC VM module")->required()->envname("EVMC_VM");
    run_cmd.add_option("--gas", msg.gas, "Execution gas limit", true)
        ->check(CLI::Range(0, 1000000000));
    run_cmd.add_option("--rev", rev, "EVM revision", true);

    try
    {
        app.parse(argc, argv);

        std::cerr << "code " << hex(code.data(), code.size()) << "\n";

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
