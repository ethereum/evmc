// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include "vmtester.hpp"

#include <evmc/loader.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>

namespace opts = boost::program_options;

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

        opts::options_description desc("EVMC VM Tester Options");
        auto add_option = desc.add_options();
        add_option("help", "Show help message");
        add_option("vm", opts::value(&vm_path)->value_name("path")->required(),
                   "Path to the VM shared library to be tested");

        opts::positional_options_description positional;
        positional.add("vm", 1);

        testing::InitGoogleTest(&argc, argv);

        opts::variables_map variables_map;
        opts::store(
            opts::command_line_parser(argc, argv).options(desc).positional(positional).run(),
            variables_map);

        if (variables_map.count("help"))
        {
            std::cout << "\n" << desc << "\n";
            return 0;
        }

        opts::notify(variables_map);

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
