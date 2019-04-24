// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "../../examples/example_host.h"
#include "../../examples/example_vm.h"

#include <evmc/evmc.hpp>
#include <evmc/helpers.hpp>

#include <gtest/gtest.h>

#include <cstring>

TEST(cpp, result)
{
    static int release_called = 0;
    release_called = 0;

    {
        EXPECT_EQ(release_called, 0);
        auto raw_result = evmc_result{};
        raw_result.output_data = static_cast<uint8_t*>(std::malloc(13));
        raw_result.release = [](const evmc_result* r) {
            std::free(const_cast<uint8_t*>(r->output_data));
            ++release_called;
        };

        auto res1 = evmc::result{raw_result};
        auto res2 = std::move(res1);

        EXPECT_EQ(release_called, 0);

        [](evmc::result) {}(std::move(res2));

        EXPECT_EQ(release_called, 1);
    }
    EXPECT_EQ(release_called, 1);
}

TEST(cpp, vm)
{
    auto vm = evmc::vm{evmc_create_example_vm()};
    EXPECT_TRUE(vm.is_abi_compatible());

    auto r = vm.set_option("verbose", "3");
    EXPECT_EQ(r, EVMC_SET_OPTION_SUCCESS);

    EXPECT_EQ(vm.name(), std::string{"example_vm"});
    EXPECT_NE(vm.version()[0], 0);

    auto ctx = evmc_context{};
    auto res = vm.execute(ctx, EVMC_MAX_REVISION, {}, nullptr, 0);
    EXPECT_EQ(res.status_code, EVMC_FAILURE);
}

TEST(cpp, vm_set_option)
{
    evmc_instance raw_instance = {EVMC_ABI_VERSION, "",      "",      nullptr,
                                  nullptr,          nullptr, nullptr, nullptr};
    raw_instance.destroy = [](evmc_instance*) {};

    auto vm = evmc::vm{&raw_instance};
    EXPECT_EQ(vm.set_option("1", "2"), EVMC_SET_OPTION_INVALID_NAME);
}

TEST(cpp, host)
{
    // Use example host to execute all methods from the C++ host wrapper.

    auto* host_context = example_host_create_context();
    auto host = evmc::HostContext{host_context};

    auto a = evmc_address{{1}};
    auto v = evmc_bytes32{{7, 7, 7}};

    EXPECT_FALSE(host.account_exists(a));

    EXPECT_EQ(host.set_storage(a, {}, v), EVMC_STORAGE_MODIFIED);
    EXPECT_EQ(host.set_storage(a, {}, v), EVMC_STORAGE_UNCHANGED);
    EXPECT_EQ(host.get_storage(a, {}), v);

    EXPECT_TRUE(is_zero(host.get_balance(a)));

    EXPECT_EQ(host.get_code_size(a), 0);
    EXPECT_EQ(host.get_code_hash(a), evmc_bytes32{});
    EXPECT_EQ(host.copy_code(a, 0, nullptr, 0), 0);

    host.selfdestruct(a, a);
    EXPECT_EQ(host.call({}).gas_left, 0);

    auto tx = host.get_tx_context();
    EXPECT_EQ(host.get_tx_context().block_number, tx.block_number);

    EXPECT_EQ(host.get_block_hash(0), evmc_bytes32{});

    host.emit_log(a, nullptr, 0, nullptr, 0);

    example_host_destroy_context(host_context);
}
