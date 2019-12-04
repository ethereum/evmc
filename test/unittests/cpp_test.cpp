// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

// The vector is not used here, but including it was causing compilation issues
// previously related to using explicit template argument (SFINAE disabled).
#include <vector>

#include "../../examples/example_precompiles_vm/example_precompiles_vm.h"
#include "../../examples/example_vm/example_vm.h"

#include <evmc/evmc.hpp>
#include <evmc/mocked_host.hpp>
#include <gtest/gtest.h>
#include <array>
#include <cstring>
#include <map>
#include <unordered_map>

class NullHost : public evmc::Host
{
public:
    bool account_exists(const evmc::address&) const noexcept final { return false; }

    evmc::bytes32 get_storage(const evmc::address&, const evmc::bytes32&) const noexcept final
    {
        return {};
    }

    evmc_storage_status set_storage(const evmc::address&,
                                    const evmc::bytes32&,
                                    const evmc::bytes32&) noexcept final
    {
        return {};
    }

    evmc::uint256be get_balance(const evmc::address&) const noexcept final { return {}; }

    size_t get_code_size(const evmc::address&) const noexcept final { return 0; }

    evmc::bytes32 get_code_hash(const evmc::address&) const noexcept final { return {}; }

    size_t copy_code(const evmc::address&, size_t, uint8_t*, size_t) const noexcept final
    {
        return 0;
    }

    void selfdestruct(const evmc::address&, const evmc::address&) noexcept final {}

    evmc::result call(const evmc_message&) noexcept final { return evmc::result{evmc_result{}}; }

    evmc_tx_context get_tx_context() const noexcept final { return {}; }

    evmc::bytes32 get_block_hash(int64_t) const noexcept final { return {}; }

    void emit_log(const evmc::address&,
                  const uint8_t*,
                  size_t,
                  const evmc::bytes32[],
                  size_t) noexcept final
    {}
};

TEST(cpp, address)
{
    evmc::address a;
    EXPECT_EQ(std::count(std::begin(a.bytes), std::end(a.bytes), 0), int{sizeof(a)});
    EXPECT_EQ(a, evmc::address{});
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
    EXPECT_EQ(std::count(std::begin(b.bytes), std::end(b.bytes), 0), int{sizeof(b)});
    EXPECT_EQ(b, evmc::bytes32{});
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
    ASSERT_EQ(addresses.size(), size_t{1});
    EXPECT_EQ(addresses.begin()->first, evmc::address{});

    std::unordered_map<evmc::address, bool> unordered_addresses;
    unordered_addresses.emplace(*addresses.begin());
    addresses.clear();
    ASSERT_EQ(unordered_addresses.size(), size_t{1});
    EXPECT_FALSE(unordered_addresses.begin()->first);

    std::map<evmc::bytes32, bool> storage;
    storage[{}] = true;
    ASSERT_EQ(storage.size(), size_t{1});
    EXPECT_EQ(storage.begin()->first, evmc::bytes32{});

    std::unordered_map<evmc::bytes32, bool> unordered_storage;
    unordered_storage.emplace(*storage.begin());
    storage.clear();
    ASSERT_EQ(unordered_storage.size(), size_t{1});
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
    static const uint8_t output = 0;
    int release_called = 0;
    {
        auto raw_result = evmc_result{};
        evmc_get_optional_storage(&raw_result)->pointer = &release_called;
        EXPECT_EQ(release_called, 0);

        raw_result.output_data = &output;
        raw_result.release = [](const evmc_result* r) {
            EXPECT_EQ(r->output_data, &output);
            ++*static_cast<int*>(evmc_get_const_optional_storage(r)->pointer);
        };
        EXPECT_EQ(release_called, 0);

        auto res1 = evmc::result{raw_result};
        auto res2 = std::move(res1);
        EXPECT_EQ(release_called, 0);

        auto f = [](evmc::result r) { EXPECT_EQ(r.output_data, &output); };
        f(std::move(res2));

        EXPECT_EQ(release_called, 1);
    }
    EXPECT_EQ(release_called, 1);
}

TEST(cpp, vm)
{
    auto vm = evmc::VM{evmc_create_example_vm()};
    EXPECT_TRUE(vm.is_abi_compatible());

    auto r = vm.set_option("verbose", "3");
    EXPECT_EQ(r, EVMC_SET_OPTION_SUCCESS);

    EXPECT_EQ(vm.name(), std::string{"example_vm"});
    EXPECT_NE(vm.version()[0], 0);

    const auto host = evmc_host_interface{};
    auto res = vm.execute(host, nullptr, EVMC_MAX_REVISION, {}, nullptr, 0);
    EXPECT_EQ(res.status_code, EVMC_FAILURE);
}

TEST(cpp, vm_capabilities)
{
    const auto vm = evmc::VM{evmc_create_example_vm()};

    EXPECT_TRUE(vm.get_capabilities() & EVMC_CAPABILITY_EVM1);
    EXPECT_TRUE(vm.get_capabilities() & EVMC_CAPABILITY_EWASM);
    EXPECT_FALSE(vm.get_capabilities() & EVMC_CAPABILITY_PRECOMPILES);
    EXPECT_TRUE(vm.has_capability(EVMC_CAPABILITY_EVM1));
    EXPECT_TRUE(vm.has_capability(EVMC_CAPABILITY_EWASM));
    EXPECT_FALSE(vm.has_capability(EVMC_CAPABILITY_PRECOMPILES));
}

TEST(cpp, vm_set_option)
{
    evmc_vm raw = {EVMC_ABI_VERSION, "", "", nullptr, nullptr, nullptr, nullptr};
    raw.destroy = [](evmc_vm*) {};

    auto vm = evmc::VM{&raw};
    EXPECT_EQ(vm.set_option("1", "2"), EVMC_SET_OPTION_INVALID_NAME);
}

TEST(cpp, vm_set_option_in_constructor)
{
    static int num_calls = 0;
    const auto set_option_method = [](evmc_vm*, const char* name, const char* value) {
        ++num_calls;
        EXPECT_STREQ(name, "o");
        EXPECT_EQ(value, std::to_string(num_calls));
        return EVMC_SET_OPTION_INVALID_NAME;
    };

    evmc_vm raw{EVMC_ABI_VERSION, "", "", nullptr, nullptr, nullptr, set_option_method};
    raw.destroy = [](evmc_vm*) {};

    const auto vm = evmc::VM{&raw, {{"o", "1"}, {"o", "2"}}};
    EXPECT_EQ(num_calls, 2);
}

TEST(cpp, vm_null)
{
    evmc::VM vm;
    EXPECT_FALSE(vm);
    EXPECT_TRUE(!vm);
}

TEST(cpp, vm_move)
{
    static int destroy_counter = 0;
    const auto template_vm = evmc_vm{
        EVMC_ABI_VERSION, "", "", [](evmc_vm*) { ++destroy_counter; }, nullptr, nullptr, nullptr};

    EXPECT_EQ(destroy_counter, 0);
    {
        auto v1 = template_vm;
        auto v2 = template_vm;

        auto vm1 = evmc::VM{&v1};
        EXPECT_TRUE(vm1);
        vm1 = evmc::VM{&v2};
        EXPECT_TRUE(vm1);
    }
    EXPECT_EQ(destroy_counter, 2);
    {
        auto v1 = template_vm;

        auto vm1 = evmc::VM{&v1};
        EXPECT_TRUE(vm1);
        vm1 = evmc::VM{};
        EXPECT_FALSE(vm1);
    }
    EXPECT_EQ(destroy_counter, 3);
    {
        auto v1 = template_vm;

        auto vm1 = evmc::VM{&v1};
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
        auto v1 = template_vm;

        auto vm1 = evmc::VM{&v1};
        auto& vm1_ref = vm1;
        vm1 = std::move(vm1_ref);
        EXPECT_EQ(destroy_counter, 5);  // Already destroyed.
        EXPECT_FALSE(vm1);              // Null.
    }
    EXPECT_EQ(destroy_counter, 5);
}

TEST(cpp, vm_execute_precompiles)
{
    auto vm = evmc::VM{evmc_create_example_precompiles_vm()};
    EXPECT_EQ(vm.get_capabilities(), evmc_capabilities_flagset{EVMC_CAPABILITY_PRECOMPILES});

    constexpr std::array<uint8_t, 3> input{{1, 2, 3}};

    evmc_message msg{};
    msg.destination.bytes[19] = 4;  // Call Identify precompile at address 0x4.
    msg.input_data = input.data();
    msg.input_size = input.size();
    msg.gas = 18;

    auto res = vm.execute(EVMC_MAX_REVISION, msg, nullptr, 0);
    EXPECT_EQ(res.status_code, EVMC_SUCCESS);
    EXPECT_EQ(res.gas_left, 0);
    ASSERT_EQ(res.output_size, input.size());
    EXPECT_TRUE(std::equal(input.begin(), input.end(), res.output_data));
}

TEST(cpp, vm_execute_with_null_host)
{
    // This tests only if the used VM::execute() overload is at least implemented.
    // We know that the example VM will not use the host context in this case.

    auto host = NullHost{};

    auto vm = evmc::VM{evmc_create_example_vm()};
    evmc_message msg{};
    auto res = vm.execute(host, EVMC_MAX_REVISION, msg, nullptr, 0);
    EXPECT_EQ(res.status_code, EVMC_FAILURE);
}

TEST(cpp, host)
{
    // Use MockedHost to execute all methods from the C++ host wrapper.
    evmc::MockedHost mockedHost;
    const auto& host_interface = evmc::MockedHost::get_interface();
    auto* host_context = mockedHost.to_context();

    auto host = evmc::HostContext{host_interface, host_context};

    const auto a = evmc::address{{{1}}};
    const auto v = evmc::bytes32{{{7, 7, 7}}};

    EXPECT_FALSE(host.account_exists(a));

    mockedHost.accounts[a].storage[{}].value.bytes[0] = 1;
    EXPECT_TRUE(host.account_exists(a));

    EXPECT_EQ(host.set_storage(a, {}, v), EVMC_STORAGE_MODIFIED);
    EXPECT_EQ(host.set_storage(a, {}, v), EVMC_STORAGE_UNCHANGED);
    EXPECT_EQ(host.get_storage(a, {}), v);

    EXPECT_TRUE(evmc::is_zero(host.get_balance(a)));

    EXPECT_EQ(host.get_code_size(a), size_t{0});
    EXPECT_EQ(host.get_code_hash(a), evmc::bytes32{});
    EXPECT_EQ(host.copy_code(a, 0, nullptr, 0), size_t{0});

    host.selfdestruct(a, a);

    auto tx = host.get_tx_context();
    EXPECT_EQ(host.get_tx_context().block_number, tx.block_number);

    EXPECT_EQ(host.get_block_hash(0), evmc::bytes32{});

    host.emit_log(a, nullptr, 0, nullptr, 0);
}

TEST(cpp, host_call)
{
    // Use example host to test Host::call() method.
    evmc::MockedHost mockedHost;
    const auto& host_interface = evmc::MockedHost::get_interface();
    auto* host_context = mockedHost.to_context();

    auto host = evmc::HostContext{};  // Use default constructor.
    host = evmc::HostContext{host_interface, host_context};

    EXPECT_EQ(host.call({}).gas_left, 0);
    ASSERT_EQ(mockedHost.recorded_calls.size(), 1u);
    const auto& recorded_msg1 = mockedHost.recorded_calls.back();
    EXPECT_EQ(recorded_msg1.kind, EVMC_CALL);
    EXPECT_EQ(recorded_msg1.gas, 0);
    EXPECT_EQ(recorded_msg1.flags, 0u);
    EXPECT_EQ(recorded_msg1.depth, 0);
    EXPECT_EQ(recorded_msg1.input_data, nullptr);
    EXPECT_EQ(recorded_msg1.input_size, 0u);

    auto msg = evmc_message{};
    msg.gas = 1;
    evmc::bytes input{0xa, 0xb, 0xc};
    msg.input_data = input.data();
    msg.input_size = input.size();

    mockedHost.call_result.status_code = EVMC_REVERT;
    mockedHost.call_result.gas_left = 4321;
    mockedHost.call_result.output_data = &input[2];
    mockedHost.call_result.output_size = 1;

    auto res = host.call(msg);
    ASSERT_EQ(mockedHost.recorded_calls.size(), 2u);
    const auto& recorded_msg2 = mockedHost.recorded_calls.back();
    EXPECT_EQ(recorded_msg2.kind, EVMC_CALL);
    EXPECT_EQ(recorded_msg2.gas, 1);
    EXPECT_EQ(recorded_msg2.flags, 0u);
    EXPECT_EQ(recorded_msg2.depth, 0);
    ASSERT_EQ(recorded_msg2.input_size, 3u);
    EXPECT_EQ(evmc::bytes(recorded_msg2.input_data, recorded_msg2.input_size), input);

    EXPECT_EQ(res.status_code, EVMC_REVERT);
    EXPECT_EQ(res.gas_left, 4321);
    ASSERT_EQ(res.output_size, 1u);
    EXPECT_EQ(*res.output_data, input[2]);
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
    EXPECT_EQ(r.output_size, size_t{0});
}

TEST(cpp, result_create)
{
    const uint8_t output[] = {1, 2};
    auto r = evmc::result{EVMC_FAILURE, -1, output, sizeof(output)};
    EXPECT_EQ(r.status_code, EVMC_FAILURE);
    EXPECT_EQ(r.gas_left, -1);
    ASSERT_TRUE(r.output_data);
    ASSERT_EQ(r.output_size, size_t{2});
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

TEST(cpp, mocked_account)
{
    using namespace evmc::literals;

    evmc::MockedAccount account;
    EXPECT_EQ(account.nonce, 0);
    --account.nonce;
    account.set_balance(0x0102030405060708);

    EXPECT_EQ(account.balance,
              0x0000000000000000000000000000000000000000000000000102030405060708_bytes32);
    EXPECT_EQ(account.nonce, -1);
}
