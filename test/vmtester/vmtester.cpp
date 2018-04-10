// EVMC -- Ethereum Client-VM Connector API
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include <evmc.h>

#include <boost/dll.hpp>
#include <boost/dll/library_info.hpp>
#include <boost/dll/shared_library.hpp>

#include <iostream>

extern "C" using evmc_create_fn = evmc_instance*();

namespace dll = boost::dll;

namespace
{
bool ends_with(const std::string& str, const std::string& ending)
{
    if (str.size() < ending.size())
        return false;

    return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}
}

std::string find_evmc_create_function(const boost::filesystem::path& plugin_path)
{
    std::cout << "Loading " << plugin_path << '\n';
    dll::library_info info(plugin_path);

    for (auto& symbol : info.symbols())
    {
        if (ends_with(symbol, "_create"))
        {
            std::cout << "Found `" << symbol << "`\n";

            return symbol;
        }
    }
}

int main(int argc, const char* argv[])
{
    if (argc > 1)
    {
        boost::filesystem::path plugin_path{argv[1]};
        auto fn_name = find_evmc_create_function(plugin_path);

        auto fn = dll::import<evmc_create_fn>(plugin_path, fn_name);

        auto vm = fn();

        std::cout << "ABI: " << vm->abi_version << "\n";

        vm->destroy(vm);
    }
    return 0;
}
