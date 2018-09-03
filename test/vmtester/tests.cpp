// EVMC: Ethereum Client-VM Connector API
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#include "../../examples/example_host.h"
#include "vmtester.hpp"

#include <evmc/helpers.h>

#include <array>
#include <cstring>

// Compile time checks:

static_assert(sizeof(evmc_uint256be) == 32, "evmc_uint256be is too big");
static_assert(sizeof(evmc_address) == 20, "evmc_address is too big");
static_assert(sizeof(evmc_result) <= 64, "evmc_result does not fit cache line");
static_assert(sizeof(evmc_instance) <= 64, "evmc_instance does not fit cache line");
static_assert(offsetof(evmc_message, value) % 8 == 0, "evmc_message.value not aligned");

// Check enums match int size.
// On GCC/clang the underlying type should be unsigned int, on MSVC int
static_assert(sizeof(evmc_call_kind) == sizeof(int),
              "Enum `evmc_call_kind` is not the size of int");
static_assert(sizeof(evmc_revision) == sizeof(int), "Enum `evmc_revision` is not the size of int");

static constexpr size_t optionalDataSize =
    sizeof(evmc_result) - offsetof(evmc_result, create_address);
static_assert(optionalDataSize == sizeof(evmc_result_optional_storage), "");


TEST_F(evmc_vm_test, abi_version_match)
{
    ASSERT_EQ(vm->abi_version, EVMC_ABI_VERSION);
}

TEST_F(evmc_vm_test, execute)
{
    evmc_context* context = example_host_create_context();
    evmc_message msg{};
    std::array<uint8_t, 2> code = {{0xfe, 0x00}};

    evmc_result result =
        vm->execute(vm, context, EVMC_LATEST_REVISION, &msg, code.data(), code.size());

    // Validate some constraints
    if (result.status_code != EVMC_SUCCESS && result.status_code != EVMC_REVERT)
    {
        EXPECT_EQ(result.gas_left, 0);
    }

    if (result.output_data == NULL)
    {
        EXPECT_EQ(result.output_size, 0);
    }

    if (result.release)
        result.release(&result);

    example_host_destroy_context(context);
}

TEST_F(evmc_vm_test, set_option_unknown)
{
    if (vm->set_option)
    {
        int r = vm->set_option(vm, "unknown_option_csk9twq", "v");
        EXPECT_EQ(r, 0);
        r = vm->set_option(vm, "unknown_option_csk9twq", "x");
        EXPECT_EQ(r, 0);
    }
}

TEST_F(evmc_vm_test, set_option_empty_value)
{
    if (vm->set_option)
    {
        int r = vm->set_option(vm, "unknown_option_csk9twq", nullptr);
        EXPECT_EQ(r, 0);
    }
}

TEST_F(evmc_vm_test, name)
{
    ASSERT_NE(vm->name, nullptr);
    EXPECT_GT(std::strlen(vm->name), 0) << "VM name cannot be empty";
}

TEST_F(evmc_vm_test, version)
{
    ASSERT_NE(vm->version, nullptr);
    EXPECT_GT(std::strlen(vm->version), 0) << "VM name cannot be empty";
}

TEST_F(evmc_vm_test, set_tracer)
{
    static const auto tracer_callback = [](evmc_tracer_context*, size_t, evmc_status_code, int64_t,
                                           size_t, const evmc_uint256be*, size_t, size_t, size_t,
                                           const uint8_t*) noexcept {};
    if (vm->set_tracer)
        vm->set_tracer(vm, tracer_callback, nullptr);
}
