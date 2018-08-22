// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#include "vmtester.hpp"

#include <evmc/loader.h>

#include <CLI/CLI.hpp>

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
        std::string vm_path;

        CLI::App app{"EVMC VM Tester"};
        app.add_option("vm", vm_path, "Path to the VM shared library to be tested");
        testing::InitGoogleTest(&argc, argv);
        CLI11_PARSE(app, argc, argv);

        std::cout << "Testing " << vm_path << "\n";
        evmc_loader_error_code ec;
        create_fn = evmc_load(vm_path.c_str(), &ec);
        switch (ec)
        {
        case EVMC_LOADER_SUCCESS:
            break;
        case EVMC_LOADER_CANNOT_OPEN:
            std::cerr << "Cannot open " << vm_path << "\n";
            return static_cast<int>(ec);
        case EVMC_LOADER_SYMBOL_NOT_FOUND:
            std::cerr << "EVMC create function not found in " << vm_path << "\n";
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
        return 1;
    }
}
