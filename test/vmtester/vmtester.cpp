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

std::vector<std::string> get_vm_names(const fs::path path)
{
    std::vector<std::string> names;

    // Get the filename without extension.
    auto name = path.stem().string();

    // Skip the optional library name prefix.
    const std::string lib_name_prefix{"lib"};
    if (name.find(lib_name_prefix) == 0)
        name = name.substr(lib_name_prefix.size());

    size_t hyphen_pos = 0;
    const std::string hyphen{"-"};
    if ((hyphen_pos = name.find(hyphen)) != std::string::npos)
    {
        // Replace the hyphen with underscore.
        name.replace(hyphen_pos, hyphen.size(), "_");
        names.emplace_back(name);

        // Also add the  name without the hyphen-separated prefix.
        names.emplace_back(name.substr(hyphen_pos + hyphen.size()));
    }
    else
    {
        // Add the filename as the name.
        names.emplace_back(std::move(name));
    }

    return names;
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

        std::cout << "Testing " << vm_path.filename().string() << "\n"
                  << "Path: " << vm_path.string() << "\n";

        for (auto&& name : get_vm_names(vm_path))
        {
            try
            {
                const std::string create_fn_name = "evmc_create_" + name;
                std::cout << "Seeking `" << create_fn_name << "`... ";
                create_fn = dll::import<evmc_create_fn>(vm_path, create_fn_name);
                std::cout << "found.\n";
                break;
            }
            catch (boost::system::system_error& err)
            {
                using namespace boost::system;
                const error_code windows_error{127, system_category()};
                constexpr auto posix_error = errc::invalid_seek;
                if (err.code() != posix_error && err.code() != windows_error)
                    throw;  // Error other than "symbol not found".
                std::cout << "not found.\n";
            }
        }

        if (!create_fn)
        {
            std::cerr << "EVMC create function not found in " << vm_path.string() << "\n";
            return 2;
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
