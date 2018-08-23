// EVMC: Ethereum Client-VM Connector API
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#pragma once

#include <evmc/evmc.h>

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
