// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "vmtester.hpp"
#include <evmc/evmc.hpp>
#include <evmc/loader.h>
#include <iostream>

evmc_vm* evmc_vm_test::vm;
evmc::VM evmc_vm_test::owned_vm;

void evmc_vm_test::init_vm(evmc_vm* _owned_vm) noexcept
{
    vm = _owned_vm;
    owned_vm = evmc::VM{_owned_vm};
}

class cli_parser
{
public:
    const char* const application_name = nullptr;
    const char* const application_version = nullptr;

    std::vector<std::string> arguments_names;
    std::vector<std::string> arguments;

    cli_parser(const char* app_name,
               const char* app_version,
               std::vector<std::string> args_names) noexcept
      : application_name{app_name},
        application_version{app_version},
        arguments_names{std::move(args_names)}
    {
        arguments.reserve(this->arguments_names.size());
    }

    /// Parses the command line arguments.
    ///
    /// It recognize --help and --version options and output for these is sent
    /// to the @p out output stream.
    /// Errors are sent to the @p err output stream.
    ///
    /// @return Negative value in case of error,
    ///         0 in case --help or --version was provided and the program should terminate,
    ///         positive value in case the program should continue.
    int parse(int argc, char* argv[], std::ostream& out, std::ostream& err)
    {
        bool help = false;
        bool version = false;
        size_t num_args = 0;
        for (int i = 1; i < argc; ++i)
        {
            auto arg = std::string{argv[i]};

            auto x = arg.find_first_not_of('-');
            if (x == 0)  // Argument.
            {
                ++num_args;
                if (num_args > arguments_names.size())
                {
                    err << "Unexpected argument \"" << arg << "\"\n";
                    return -1;
                }
                arguments.emplace_back(std::move(arg));
                continue;
            }
            else if (x <= 2)
            {
                arg.erase(0, x);

                if (arg == "version")
                {
                    version = true;
                    continue;
                }
                if (arg == "help")
                {
                    help = true;
                    continue;
                }
            }

            err << "Unknown option \"" << argv[i] << "\"\n";
            return -1;
        }

        out << application_name << " " << application_version << "\n";

        if (help)
        {
            out << "Usage: " << argv[0];
            for (const auto& name : arguments_names)
                out << " " << name;
            out << "\n";
            return 0;
        }

        if (version)
            return 0;

        if (num_args < arguments_names.size())
        {
            for (auto i = num_args; i < arguments_names.size(); ++i)
                err << "The " << arguments_names[i] << " argument is required.\n";
            err << "Run with --help for more information.\n";
            return -1;
        }

        return 1;
    }
};

int main(int argc, char* argv[])
{
    try
    {
        testing::InitGoogleTest(&argc, argv);

        auto cli = cli_parser{"EVMC VM Tester", PROJECT_VERSION, {"MODULE"}};

        const auto error_code = cli.parse(argc, argv, std::cout, std::cerr);
        if (error_code <= 0)
            return error_code;

        const auto& evmc_module = cli.arguments[0];
        std::cout << "Testing " << evmc_module << "\n";

        evmc_loader_error_code ec;
        auto vm_instance = evmc_load_and_configure(evmc_module.c_str(), &ec);
        if (ec != EVMC_LOADER_SUCCESS)
        {
            const auto error = evmc_last_error_msg();
            if (error != nullptr)
                std::cerr << error << "\n";
            else
                std::cerr << "Loading error " << ec << "\n";
            return static_cast<int>(ec);
        }

        evmc_vm_test::init_vm(vm_instance);

        std::cout << std::endl;
        return RUN_ALL_TESTS();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
        return -2;
    }
}
