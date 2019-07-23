// EVMC: Ethereum Client-VM Connector API
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <evmc/evmc.h>
#include <gtest/gtest.h>

class evmc_vm_test : public ::testing::Test
{
public:
    /// The raw pointer to the loaded VM instance.
    /// The C API is used to allow more sophisticated unit tests.
    static evmc_instance* vm;

protected:
    void SetUp() override { ASSERT_TRUE(vm != nullptr) << "VM instance not loaded"; }
};
