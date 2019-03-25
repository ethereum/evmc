// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "vmtester.hpp"

#include <evmc/loader.h>

#include <iostream>
#include <memory>

namespace
{
evmc_create_fn create_fn;

std::unique_ptr<evmc_instance, evmc_destroy_fn> create_vm()
{
    auto vm = create_fn();
    return {vm, vm->destroy};
}
}  // namespace

evmc_instance* get_vm_instance()
{
    static auto vm = create_vm();
    return vm.get();
}

class cli_parser
{
public:
    const char* const app_name = nullptr;
    const char* const app_version = nullptr;

    std::vector<std::string> args_names;
    std::vector<std::string> args;

    cli_parser(const char* app_name,
               const char* app_version,
               std::vector<std::string> args_names) noexcept
      : app_name{app_name}, app_version{app_version}, args_names{std::move(args_names)}
    {
        args.reserve(this->args_names.size());
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
                if (num_args > args_names.size())
                {
                    err << "Unexpected argument \"" << arg << "\"\n";
                    return -1;
                }
                args.emplace_back(std::move(arg));
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

        out << app_name << " " << app_version << "\n";

        if (help)
        {
            out << "Usage: " << argv[0];
            for (const auto& name : args_names)
                out << " " << name;
            out << "\n";
            return 0;
        }

        if (version)
            return 0;

        if (num_args < args_names.size())
        {
            for (auto i = num_args; i < args_names.size(); ++i)
                err << "The " << args_names[i] << " argument is required.\n";
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

        const auto& evmc_module = cli.args[0];
        std::cout << "Testing " << evmc_module << "\n";
        evmc_loader_error_code ec;
        create_fn = evmc_load(evmc_module.c_str(), &ec);
        switch (ec)
        {
        case EVMC_LOADER_SUCCESS:
            break;
        case EVMC_LOADER_CANNOT_OPEN:
            std::cerr << "Cannot open " << evmc_module << "\n";
            return static_cast<int>(ec);
        case EVMC_LOADER_SYMBOL_NOT_FOUND:
            std::cerr << "EVMC create function not found in " << evmc_module << "\n";
            return static_cast<int>(ec);
        case EVMC_LOADER_INVALID_ARGUMENT:
            std::cerr << "Invalid argument: \"" << evmc_module << "\"\n";
            return static_cast<int>(ec);
        default:
            std::cerr << "Unexpected error in evmc_load(): " << ec << "\n";
            return static_cast<int>(ec);
        }

        std::cout << std::endl;
        return RUN_ALL_TESTS();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
        return -2;
    }
}
