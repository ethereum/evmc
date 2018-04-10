// EVMC -- Ethereum Client-VM Connector API
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include <evmc.h>
#include <gtest/gtest.h>

evmc_instance* get_vm_instance();

class evmc_vm_test : public ::testing::Test
{
protected:
    evmc_instance* vm = nullptr;

    void SetUp() override
    {
        vm = get_vm_instance();
        ASSERT_NE(vm, nullptr);
    }
};
