// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

// The vector is not used here, but including it was causing compilation issues
// previously related to using explicit template argument (SFINAE disabled).
#include <vector>

#include "../../examples/example_host.h"
#include "../../examples/example_vm/example_vm.h"

#include <evmc/evmc.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <map>
#include <unordered_map>


TEST(cpp, address)
{
    evmc::address a;
    EXPECT_EQ(std::count(std::begin(a.bytes), std::end(a.bytes), 0), sizeof(a));
    EXPECT_TRUE(is_zero(a));
    EXPECT_FALSE(a);
    EXPECT_TRUE(!a);

    auto other = evmc_address{};
    other.bytes[19] = 0xfe;
    a = other;
    EXPECT_TRUE(std::equal(std::begin(a.bytes), std::end(a.bytes), std::begin(other.bytes)));

    a.bytes[0] = 1;
    other = a;
    EXPECT_TRUE(std::equal(std::begin(a.bytes), std::end(a.bytes), std::begin(other.bytes)));
    EXPECT_FALSE(is_zero(a));
    EXPECT_TRUE(a);
    EXPECT_FALSE(!a);
}

TEST(cpp, bytes32)
{
    evmc::bytes32 b;
    EXPECT_EQ(std::count(std::begin(b.bytes), std::end(b.bytes), 0), sizeof(b));
    EXPECT_TRUE(is_zero(b));
    EXPECT_FALSE(b);
    EXPECT_TRUE(!b);

    auto other = evmc_bytes32{};
    other.bytes[31] = 0xfe;
    b = other;
    EXPECT_TRUE(std::equal(std::begin(b.bytes), std::end(b.bytes), std::begin(other.bytes)));

    b.bytes[0] = 1;
    other = b;
    EXPECT_TRUE(std::equal(std::begin(b.bytes), std::end(b.bytes), std::begin(other.bytes)));
    EXPECT_FALSE(is_zero(b));
    EXPECT_TRUE(b);
    EXPECT_FALSE(!b);
}

TEST(cpp, std_hash)
{
#pragma warning(push)
#pragma warning(disable : 4307 /* integral constant overflow */)
#pragma warning(disable : 4309 /* 'static_cast': truncation of constant value */)

#if !defined(_MSC_VER) || (_MSC_VER >= 1910 /* Only for Visual Studio 2017+ */)
    static_assert(std::hash<evmc::address>{}({}) == static_cast<size_t>(0xd94d12186c0f2fb7), "");
    static_assert(std::hash<evmc::bytes32>{}({}) == static_cast<size_t>(0x4d25767f9dce13f5), "");
#endif

    EXPECT_EQ(std::hash<evmc::address>{}({}), static_cast<size_t>(0xd94d12186c0f2fb7));
    EXPECT_EQ(std::hash<evmc::bytes32>{}({}), static_cast<size_t>(0x4d25767f9dce13f5));

    auto ea = evmc::address{};
    std::fill_n(ea.bytes, sizeof(ea), uint8_t{0xee});
    EXPECT_EQ(std::hash<evmc::address>{}(ea), static_cast<size_t>(0x41dc0178e01b7cd9));

    auto eb = evmc::bytes32{};
    std::fill_n(eb.bytes, sizeof(eb), uint8_t{0xee});
    EXPECT_EQ(std::hash<evmc::bytes32>{}(eb), static_cast<size_t>(0xbb14e5c56b477375));

#pragma warning(pop)
}

TEST(cpp, std_maps)
{
    std::map<evmc::address, bool> addresses;
    addresses[{}] = true;
    ASSERT_EQ(addresses.size(), 1);
    EXPECT_EQ(addresses.begin()->first, evmc::address{});

    std::unordered_map<evmc::address, bool> unordered_addresses;
    unordered_addresses.emplace(*addresses.begin());
    addresses.clear();
    EXPECT_EQ(unordered_addresses.size(), 1);
    EXPECT_FALSE(unordered_addresses.begin()->first);

    std::map<evmc::bytes32, bool> storage;
    storage[{}] = true;
    ASSERT_EQ(storage.size(), 1);
    EXPECT_EQ(storage.begin()->first, evmc::bytes32{});

    std::unordered_map<evmc::bytes32, bool> unordered_storage;
    unordered_storage.emplace(*storage.begin());
    storage.clear();
    EXPECT_EQ(unordered_storage.size(), 1);
    EXPECT_FALSE(unordered_storage.begin()->first);
}

TEST(cpp, address_comparison)
{
    const auto zero = evmc::address{};
    for (size_t i = 0; i < sizeof(evmc::address); ++i)
    {
        auto t = evmc::address{};
        t.bytes[i] = 1;
        auto u = evmc::address{};
        u.bytes[i] = 2;
        auto f = evmc::address{};
        f.bytes[i] = 0xff;

        EXPECT_TRUE(zero < t);
        EXPECT_TRUE(zero < u);
        EXPECT_TRUE(zero < f);
        EXPECT_TRUE(zero != t);
        EXPECT_TRUE(zero != u);
        EXPECT_TRUE(zero != f);

        EXPECT_TRUE(t < u);
        EXPECT_TRUE(t < f);
        EXPECT_TRUE(u < f);

        EXPECT_FALSE(u < t);
        EXPECT_FALSE(f < t);
        EXPECT_FALSE(f < u);

        EXPECT_TRUE(t != u);
        EXPECT_TRUE(t != f);
        EXPECT_TRUE(u != t);
        EXPECT_TRUE(u != f);
        EXPECT_TRUE(f != t);
        EXPECT_TRUE(f != u);

        EXPECT_TRUE(t == t);
        EXPECT_TRUE(u == u);
        EXPECT_TRUE(f == f);
    }
}

TEST(cpp, bytes32_comparison)
{
    const auto zero = evmc::bytes32{};
    for (size_t i = 0; i < sizeof(evmc::bytes32); ++i)
    {
        auto t = evmc::bytes32{};
        t.bytes[i] = 1;
        auto u = evmc::bytes32{};
        u.bytes[i] = 2;
        auto f = evmc::bytes32{};
        f.bytes[i] = 0xff;

        EXPECT_TRUE(zero < t);
        EXPECT_TRUE(zero < u);
        EXPECT_TRUE(zero < f);
        EXPECT_TRUE(zero != t);
        EXPECT_TRUE(zero != u);
        EXPECT_TRUE(zero != f);

        EXPECT_TRUE(t < u);
        EXPECT_TRUE(t < f);
        EXPECT_TRUE(u < f);

        EXPECT_FALSE(u < t);
        EXPECT_FALSE(f < t);
        EXPECT_FALSE(f < u);

        EXPECT_TRUE(t != u);
        EXPECT_TRUE(t != f);
        EXPECT_TRUE(u != t);
        EXPECT_TRUE(u != f);
        EXPECT_TRUE(f != t);
        EXPECT_TRUE(f != u);

        EXPECT_TRUE(t == t);
        EXPECT_TRUE(u == u);
        EXPECT_TRUE(f == f);
    }
}

TEST(cpp, literals)
{
    using namespace evmc::literals;

#if !defined(_MSC_VER) || (_MSC_VER >= 1910 /* Only for Visual Studio 2017+ */)
    constexpr auto address1 = 0xa0a1a2a3a4a5a6a7a8a9d0d1d2d3d4d5d6d7d8d9_address;
    constexpr auto hash1 =
        0x01020304050607080910a1a2a3a4a5a6a7a8a9b0c1c2c3c4c5c6c7c8c9d0d1d2_bytes32;
    constexpr auto zero_address = 0_address;
    constexpr auto zero_hash = 0_bytes32;

    static_assert(address1.bytes[0] == 0xa0, "");
    static_assert(address1.bytes[9] == 0xa9, "");
    static_assert(address1.bytes[10] == 0xd0, "");
    static_assert(address1.bytes[19] == 0xd9, "");
    static_assert(hash1.bytes[0] == 0x01, "");
    static_assert(hash1.bytes[10] == 0xa1, "");
    static_assert(hash1.bytes[31] == 0xd2, "");
    static_assert(zero_address == evmc::address{}, "");
    static_assert(zero_hash == evmc::bytes32{}, "");
#endif

    EXPECT_EQ(0_address, evmc::address{});
    EXPECT_EQ(0_bytes32, evmc::bytes32{});

    auto a1 = 0xa0a1a2a3a4a5a6a7a8a9d0d1d2d3d4d5d6d7d8d9_address;
    evmc::address e1{{{0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
                       0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9}}};
    EXPECT_EQ(a1, e1);

    auto h1 = 0x01020304050607080910a1a2a3a4a5a6a7a8a9b0c1c2c3c4c5c6c7c8c9d0d1d2_bytes32;
    evmc::bytes32 f1{{{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0xa1,
                       0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xb0, 0xc1, 0xc2,
                       0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xd0, 0xd1, 0xd2}}};
    EXPECT_EQ(h1, f1);
}

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

    EXPECT_TRUE(vm.get_capabilities() & EVMC_CAPABILITY_EVM1);
}

TEST(cpp, vm_set_option)
{
    evmc_instance raw_instance = {EVMC_ABI_VERSION, "",      "",      nullptr,
                                  nullptr,          nullptr, nullptr, nullptr};
    raw_instance.destroy = [](evmc_instance*) {};

    auto vm = evmc::vm{&raw_instance};
    EXPECT_EQ(vm.set_option("1", "2"), EVMC_SET_OPTION_INVALID_NAME);
}

TEST(cpp, vm_null)
{
    evmc::vm vm;
    EXPECT_FALSE(vm);
    EXPECT_TRUE(!vm);
}

TEST(cpp, vm_move)
{
    static int destroy_counter = 0;
    const auto template_instance =
        evmc_instance{EVMC_ABI_VERSION, "",      "",      [](evmc_instance*) { ++destroy_counter; },
                      nullptr,          nullptr, nullptr, nullptr};

    EXPECT_EQ(destroy_counter, 0);
    {
        auto v1 = template_instance;
        auto v2 = template_instance;

        auto vm1 = evmc::vm{&v1};
        EXPECT_TRUE(vm1);
        vm1 = evmc::vm{&v2};
        EXPECT_TRUE(vm1);
    }
    EXPECT_EQ(destroy_counter, 2);
    {
        auto v1 = template_instance;

        auto vm1 = evmc::vm{&v1};
        EXPECT_TRUE(vm1);
        vm1 = evmc::vm{};
        EXPECT_FALSE(vm1);
    }
    EXPECT_EQ(destroy_counter, 3);
    {
        auto v1 = template_instance;

        auto vm1 = evmc::vm{&v1};
        EXPECT_TRUE(vm1);
        auto vm2 = std::move(vm1);
        EXPECT_TRUE(vm2);
        EXPECT_FALSE(vm1);  // NOLINT
        auto vm3 = std::move(vm2);
        EXPECT_TRUE(vm3);
        EXPECT_FALSE(vm2);  // NOLINT
        EXPECT_FALSE(vm1);
    }
    EXPECT_EQ(destroy_counter, 4);
    {
        // Moving to itself will destroy the VM and reset the evmc::vm.
        auto v1 = template_instance;

        auto vm1 = evmc::vm{&v1};
        auto& vm1_ref = vm1;
        vm1 = std::move(vm1_ref);
        EXPECT_EQ(destroy_counter, 5);  // Already destroyed.
        EXPECT_FALSE(vm1);              // Null.
    }
    EXPECT_EQ(destroy_counter, 5);
}

TEST(cpp, host)
{
    // Use example host to execute all methods from the C++ host wrapper.

    auto* host_context = example_host_create_context(evmc_tx_context{});
    auto host = evmc::HostContext{host_context};

    const auto a = evmc::address{{{1}}};
    const auto v = evmc::bytes32{{{7, 7, 7}}};

    EXPECT_FALSE(host.account_exists(a));

    EXPECT_EQ(host.set_storage(a, {}, v), EVMC_STORAGE_MODIFIED);
    EXPECT_EQ(host.set_storage(a, {}, v), EVMC_STORAGE_UNCHANGED);
    EXPECT_EQ(host.get_storage(a, {}), v);

    EXPECT_TRUE(evmc::is_zero(host.get_balance(a)));

    EXPECT_EQ(host.get_code_size(a), 0);
    EXPECT_EQ(host.get_code_hash(a), evmc::bytes32{});
    EXPECT_EQ(host.copy_code(a, 0, nullptr, 0), 0);

    host.selfdestruct(a, a);

    auto tx = host.get_tx_context();
    EXPECT_EQ(host.get_tx_context().block_number, tx.block_number);

    EXPECT_EQ(host.get_block_hash(0), evmc::bytes32{});

    host.emit_log(a, nullptr, 0, nullptr, 0);

    example_host_destroy_context(host_context);
}

TEST(cpp, host_call)
{
    // Use example host to test Host::call() method.

    auto* host_context = example_host_create_context(evmc_tx_context{});
    auto host = evmc::HostContext{host_context};

    EXPECT_EQ(host.call({}).gas_left, 0);

    auto msg = evmc_message{};
    msg.gas = 1;
    uint8_t input[] = {0xa, 0xb, 0xc};
    msg.input_data = input;
    msg.input_size = sizeof(input);

    auto res = host.call(msg);
    EXPECT_EQ(res.status_code, EVMC_REVERT);
    EXPECT_EQ(res.output_size, msg.input_size);
    EXPECT_TRUE(std::equal(&res.output_data[0], &res.output_data[res.output_size], msg.input_data));

    example_host_destroy_context(host_context);
}

TEST(cpp, result_raii)
{
    static auto release_called = 0;
    release_called = 0;
    auto release_fn = [](const evmc_result*) noexcept { ++release_called; };

    {
        auto raw_result = evmc_result{};
        raw_result.status_code = EVMC_INTERNAL_ERROR;
        raw_result.release = release_fn;

        auto raii_result = evmc::result{raw_result};
        EXPECT_EQ(raii_result.status_code, EVMC_INTERNAL_ERROR);
        EXPECT_EQ(raii_result.gas_left, 0);
        raii_result.gas_left = -1;

        auto raw_result2 = raii_result.release_raw();
        EXPECT_EQ(raw_result2.status_code, EVMC_INTERNAL_ERROR);
        EXPECT_EQ(raw_result.status_code, EVMC_INTERNAL_ERROR);
        EXPECT_EQ(raw_result2.gas_left, -1);
        EXPECT_EQ(raw_result.gas_left, 0);
        EXPECT_EQ(raw_result2.release, release_fn);
        EXPECT_EQ(raw_result.release, release_fn);
    }
    EXPECT_EQ(release_called, 0);

    {
        auto raw_result = evmc_result{};
        raw_result.status_code = EVMC_INTERNAL_ERROR;
        raw_result.release = release_fn;

        auto raii_result = evmc::result{raw_result};
        EXPECT_EQ(raii_result.status_code, EVMC_INTERNAL_ERROR);
    }
    EXPECT_EQ(release_called, 1);
}

TEST(cpp, result_move)
{
    static auto release_called = 0;
    auto release_fn = [](const evmc_result*) noexcept { ++release_called; };

    release_called = 0;
    {
        auto raw = evmc_result{};
        raw.gas_left = -1;
        raw.release = release_fn;

        auto r0 = evmc::result{raw};
        EXPECT_EQ(r0.gas_left, raw.gas_left);

        auto r1 = std::move(r0);
        EXPECT_EQ(r1.gas_left, raw.gas_left);
    }
    EXPECT_EQ(release_called, 1);

    release_called = 0;
    {
        auto raw1 = evmc_result{};
        raw1.gas_left = 1;
        raw1.release = release_fn;

        auto raw2 = evmc_result{};
        raw2.gas_left = 1;
        raw2.release = release_fn;

        auto r1 = evmc::result{raw1};
        auto r2 = evmc::result{raw2};

        r2 = std::move(r1);
    }
    EXPECT_EQ(release_called, 2);
}

TEST(cpp, result_create_no_output)
{
    auto r = evmc::result{EVMC_REVERT, 1, nullptr, 0};
    EXPECT_EQ(r.status_code, EVMC_REVERT);
    EXPECT_EQ(r.gas_left, 1);
    EXPECT_FALSE(r.output_data);
    EXPECT_EQ(r.output_size, 0);
}

TEST(cpp, result_create)
{
    const uint8_t output[] = {1, 2};
    auto r = evmc::result{EVMC_FAILURE, -1, output, sizeof(output)};
    EXPECT_EQ(r.status_code, EVMC_FAILURE);
    EXPECT_EQ(r.gas_left, -1);
    ASSERT_TRUE(r.output_data);
    ASSERT_EQ(r.output_size, 2);
    EXPECT_EQ(r.output_data[0], 1);
    EXPECT_EQ(r.output_data[1], 2);

    auto c = evmc::make_result(r.status_code, r.gas_left, r.output_data, r.output_size);
    EXPECT_EQ(c.status_code, r.status_code);
    EXPECT_EQ(c.gas_left, r.gas_left);
    ASSERT_EQ(c.output_size, r.output_size);
    EXPECT_EQ(evmc::address{c.create_address}, evmc::address{r.create_address});
    ASSERT_TRUE(c.release);
    EXPECT_TRUE(std::memcmp(c.output_data, r.output_data, c.output_size) == 0);
    c.release(&c);
}