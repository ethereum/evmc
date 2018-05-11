// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include "vmtester.hpp"

#include <boost/dll.hpp>
#include <boost/dll/alias.hpp>
#include <boost/function.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <memory>

namespace fs = boost::filesystem;
namespace dll = boost::dll;
namespace opts = boost::program_options;

extern "C" using evmc_create_fn = evmc_instance * ();

namespace
{
boost::function<evmc_create_fn> create_fn;

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
        fs::path vm_path;

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

        auto symbols = dll::library_info{vm_path}.symbols();
        auto it = std::find_if(symbols.begin(), symbols.end(), [](const std::string& symbol) {
            return symbol.find("evmc_create_") == 0;
        });
        if (it == symbols.end())
        {
            std::cerr << "EVMC create function not found in " << vm_path.string() << "\n";
            return 2;
        }
        const std::string& create_fn_name = *it;

        std::cout << "Testing " << vm_path.filename().string() << "\n  "
                  << "Path: " << vm_path.string() << "\n  "
                  << "Create function: " << create_fn_name << "()\n\n";

        create_fn = dll::import<evmc_create_fn>(vm_path, create_fn_name);

        return RUN_ALL_TESTS();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
