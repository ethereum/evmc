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

int main(int argc, char* argv[])
{
    try
    {
        const auto app_name = "EVMC VM Tester " PROJECT_VERSION;

        testing::InitGoogleTest(&argc, argv);

        auto help = false;
        auto version = false;

        std::string evmc_module;
        int num_args = 0;
        for (int i = 1; i < argc; ++i)
        {
            auto arg = std::string{argv[i]};

            auto x = arg.find_first_not_of('-');
            if (x == 0)  // Argument.
            {
                ++num_args;
                if (num_args > 1)
                {
                    std::cerr << "Unexpected argument \"" << arg << "\"\n";
                    return -1;
                }
                evmc_module = std::move(arg);
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

            std::cerr << "Unknown option \"" << argv[i] << "\"\n";
            return -1;
        }

        if (help)
        {
            std::cout << "\n"
                      << app_name << "\n"
                      << "Usage: " << argv[0] << " MODULE\n";
            return 0;
        }

        if (version)
        {
            std::cout << app_name << "\n";
            return 0;
        }

        if (num_args < 1)
        {
            std::cerr << "The MODULE argument is required.\n"
                      << "Run with --help for more information.\n";
            return -1;
        }

        std::cout << app_name << "\n";

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
