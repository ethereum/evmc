// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/mocked_host.hpp>
#include <gtest/gtest.h>

using namespace evmc::literals;

TEST(mocked_host, mocked_account)
{
    evmc::MockedAccount account;
    EXPECT_EQ(account.nonce, 0);
    --account.nonce;
    account.set_balance(0x0102030405060708);

    EXPECT_EQ(account.balance,
              0x0000000000000000000000000000000000000000000000000102030405060708_bytes32);
    EXPECT_EQ(account.nonce, -1);
}

TEST(mocked_host, storage)
{
    const auto addr1 = evmc::address{};
    const auto addr2 = 0x2000000000000000000000000000000000000000_address;
    const auto val1 = evmc::bytes32{};
    const auto val2 = 0x2000000000000000000000000000000000000000000000000102030405060708_bytes32;
    const auto val3 = 0x1000000000000000000000000000000000000000000000000000000000000000_bytes32;

    evmc::MockedHost host;
    const auto& chost = host;

    // Null bytes returned for non-existing accounts.
    EXPECT_EQ(chost.get_storage(addr1, {}), evmc::bytes32{});
    EXPECT_EQ(chost.get_storage(addr2, {}), evmc::bytes32{});

    // Set storage on non-existing account creates the account.
    EXPECT_EQ(host.set_storage(addr1, val1, val2), EVMC_STORAGE_ADDED);
    EXPECT_EQ(chost.accounts.count(addr1), 1u);
    EXPECT_EQ(host.accounts[addr1].storage.count(val1), 1u);
    EXPECT_EQ(host.accounts[addr1].storage[val1].current, val2);

    auto& acc2 = host.accounts[addr2];
    EXPECT_EQ(chost.get_storage(addr2, val1), evmc::bytes32{});
    EXPECT_EQ(acc2.storage.size(), 0u);
    EXPECT_EQ(host.set_storage(addr2, val1, val2), EVMC_STORAGE_ADDED);
    EXPECT_EQ(chost.get_storage(addr2, val1), val2);
    EXPECT_EQ(acc2.storage.count(val1), 1u);
    EXPECT_EQ(host.set_storage(addr2, val1, val2), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(chost.get_storage(addr2, val1), val2);
    EXPECT_EQ(acc2.storage.count(val1), 1u);
    EXPECT_EQ(host.set_storage(addr2, val1, val3), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(chost.get_storage(addr2, val1), val3);
    EXPECT_EQ(acc2.storage.count(val1), 1u);
    EXPECT_NE(acc2.storage[val1].current, acc2.storage[val1].original);
    EXPECT_EQ(host.set_storage(addr2, val1, val1), EVMC_STORAGE_ADDED_DELETED);
    EXPECT_EQ(chost.get_storage(addr2, val1), val1);
    EXPECT_EQ(acc2.storage.count(val1), 1u);
    EXPECT_EQ(acc2.storage.size(), 1u);
    EXPECT_EQ(acc2.storage[val1].current, acc2.storage[val1].original);

    EXPECT_EQ(chost.get_storage(addr2, val3), evmc::bytes32{});
    acc2.storage[val3] = val2;
    EXPECT_EQ(chost.get_storage(addr2, val3), val2);
    EXPECT_EQ(acc2.storage.find(val3)->second.current, acc2.storage.find(val3)->second.original);
    EXPECT_EQ(host.set_storage(addr2, val3, val2), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(chost.get_storage(addr2, val3), val2);
    EXPECT_EQ(host.set_storage(addr2, val3, val3), EVMC_STORAGE_MODIFIED);
    EXPECT_EQ(chost.get_storage(addr2, val3), val3);
    acc2.storage[val3].original = acc2.storage[val3].current;
    EXPECT_EQ(host.set_storage(addr2, val3, val1), EVMC_STORAGE_DELETED);
    EXPECT_EQ(chost.get_storage(addr2, val3), val1);
}

TEST(mocked_host, storage_update_scenarios)
{
    static constexpr auto addr = 0xff_address;
    static constexpr auto key = 0xfe_bytes32;

    static constexpr auto execute_scenario = [](const evmc::bytes32& original,
                                                const evmc::bytes32& current,
                                                const evmc::bytes32& value) {
        evmc::MockedHost host;
        host.accounts[addr].storage[key] = {current, original};
        return host.set_storage(addr, key, value);
    };

    static constexpr auto O = 0x00_bytes32;
    static constexpr auto X = 0x01_bytes32;
    static constexpr auto Y = 0x02_bytes32;
    static constexpr auto Z = 0x03_bytes32;

    EXPECT_EQ(execute_scenario(O, O, O), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(execute_scenario(X, O, O), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(execute_scenario(O, Y, Y), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(execute_scenario(X, Y, Y), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(execute_scenario(Y, Y, Y), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(execute_scenario(O, Y, Z), EVMC_STORAGE_ASSIGNED);
    EXPECT_EQ(execute_scenario(X, Y, Z), EVMC_STORAGE_ASSIGNED);

    EXPECT_EQ(execute_scenario(O, O, Z), EVMC_STORAGE_ADDED);
    EXPECT_EQ(execute_scenario(X, X, O), EVMC_STORAGE_DELETED);
    EXPECT_EQ(execute_scenario(X, X, Z), EVMC_STORAGE_MODIFIED);
    EXPECT_EQ(execute_scenario(X, O, Z), EVMC_STORAGE_DELETED_ADDED);
    EXPECT_EQ(execute_scenario(X, Y, O), EVMC_STORAGE_MODIFIED_DELETED);
    EXPECT_EQ(execute_scenario(X, O, X), EVMC_STORAGE_DELETED_RESTORED);
    EXPECT_EQ(execute_scenario(O, Y, O), EVMC_STORAGE_ADDED_DELETED);
    EXPECT_EQ(execute_scenario(X, Y, X), EVMC_STORAGE_MODIFIED_RESTORED);
}

TEST(mocked_host, selfdestruct)
{
    evmc::MockedHost host;
    EXPECT_TRUE(host.recorded_selfdestructs.empty());

    EXPECT_TRUE(host.selfdestruct(0xdead01_address, 0xbece01_address));
    ASSERT_EQ(host.recorded_selfdestructs[0xdead01_address].size(), 1u);
    EXPECT_EQ(host.recorded_selfdestructs[0xdead01_address][0], 0xbece01_address);

    EXPECT_FALSE(host.selfdestruct(0xdead01_address, 0xbece02_address));
    ASSERT_EQ(host.recorded_selfdestructs[0xdead01_address].size(), 2u);
    EXPECT_EQ(host.recorded_selfdestructs[0xdead01_address][0], 0xbece01_address);
    EXPECT_EQ(host.recorded_selfdestructs[0xdead01_address][1], 0xbece02_address);

    EXPECT_TRUE(host.selfdestruct(0xdead02_address, 0xbece01_address));
    ASSERT_EQ(host.recorded_selfdestructs[0xdead02_address].size(), 1u);
    EXPECT_EQ(host.recorded_selfdestructs[0xdead02_address][0], 0xbece01_address);

    EXPECT_FALSE(host.selfdestruct(0xdead02_address, 0xbece01_address));
    ASSERT_EQ(host.recorded_selfdestructs[0xdead02_address].size(), 2u);
    EXPECT_EQ(host.recorded_selfdestructs[0xdead02_address][0], 0xbece01_address);
    EXPECT_EQ(host.recorded_selfdestructs[0xdead02_address][1], 0xbece01_address);
}
