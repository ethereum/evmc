// EVMC: Ethereum Client-VM Connector API
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <evmc/evmc.hpp>
#include <gtest/gtest.h>

class evmc_vm_test : public ::testing::Test
{
public:
    static void init_vm(evmc_instance* owned_vm_instance) noexcept;

protected:
    /// The raw pointer to the loaded VM instance.
    /// The C API is used to allow more sophisticated unit tests.
    static evmc_instance* vm;

    /// The C++ RAII wrapper of the loaded VM instance.
    static evmc::vm owned_vm;

    void SetUp() override { ASSERT_TRUE(vm != nullptr) << "VM instance not loaded"; }
};
