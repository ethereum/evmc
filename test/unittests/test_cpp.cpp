// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include "../../examples/example_vm.h"

#include <evmc/evmc.hpp>

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
