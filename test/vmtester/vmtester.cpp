// EVMC -- Ethereum Client-VM Connector API
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include "vmtester.hpp"

#include <evmc.h>

#include <gtest/gtest.h>

#include <boost/dll.hpp>
#include <boost/dll/library_info.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <memory>

namespace fs = boost::filesystem;
namespace dll = boost::dll;

extern "C" using evmc_create_fn = evmc_instance*();

namespace
{
boost::function<evmc_create_fn> create_fn;

bool ends_with(const std::string& str, const std::string& ending)
{
    if (str.size() < ending.size())
        return false;

    return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}

std::unique_ptr<evmc_instance, evmc_destroy_fn> create_vm()
{
    auto vm = create_fn();
    return {vm, vm->destroy};
}
}

evmc_instance* get_vm_instance()
{

    static auto vm = create_vm();
    return vm.get();
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    if (argc < 2)
        return 1;

    boost::filesystem::path plugin_path{argv[1]};

    auto symbols = dll::library_info{plugin_path}.symbols();
    auto it = std::find_if(symbols.begin(), symbols.end(),
        [](const std::string& symbol) { return ends_with(symbol, "_create"); });
    if (it == symbols.end())
        return 2;

    std::cout << "Testing\n  " << argv[1] << "\n  " << *it << "()\n\n";

    create_fn = dll::import<evmc_create_fn>(plugin_path, *it);

    return RUN_ALL_TESTS();
}
