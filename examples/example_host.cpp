/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

/// @file
/// Example implementation of an EVMC Host.

#include "example_host.h"

#include <evmc/evmc.hpp>
#include <evmc/helpers.hpp>

#include <map>

struct account
{
    evmc_uint256be balance = {};
    size_t code_size = 0;
    evmc_bytes32 code_hash = {};
    std::map<evmc_bytes32, evmc_bytes32> storage;
};

class ExampleHost : public evmc::Host
{
    std::map<evmc_address, account> accounts;

public:
    bool account_exists(const evmc_address& addr) noexcept final
    {
        return accounts.find(addr) != accounts.end();
    }

    evmc_bytes32 get_storage(const evmc_address& addr, const evmc_bytes32& key) noexcept final
    {
        auto it = accounts.find(addr);
        if (it != accounts.end())
            return it->second.storage[key];
        return {};
    }

    evmc_storage_status set_storage(const evmc_address& addr,
                                    const evmc_bytes32& key,
                                    const evmc_bytes32& value) noexcept final
    {
        auto& account = accounts[addr];
        auto prev_value = account.storage[key];
        account.storage[key] = value;

        return (prev_value == value) ? EVMC_STORAGE_UNCHANGED : EVMC_STORAGE_MODIFIED;
    }

    evmc_uint256be get_balance(const evmc_address& addr) noexcept final
    {
        auto it = accounts.find(addr);
        if (it != accounts.end())
            return it->second.balance;
        return {};
    }

    size_t get_code_size(const evmc_address& addr) noexcept final
    {
        auto it = accounts.find(addr);
        if (it != accounts.end())
            return it->second.code_size;
        return 0;
    }

    evmc_bytes32 get_code_hash(const evmc_address& addr) noexcept final
    {
        auto it = accounts.find(addr);
        if (it != accounts.end())
            return it->second.code_hash;
        return {};
    }

    size_t copy_code(const evmc_address& addr,
                     size_t code_offset,
                     uint8_t* buffer_data,
                     size_t buffer_size) noexcept final
    {
        (void)addr;
        (void)code_offset;
        (void)buffer_data;
        (void)buffer_size;
        return 0;
    }

    void selfdestruct(const evmc_address& addr, const evmc_address& beneficiary) noexcept final
    {
        (void)addr;
        (void)beneficiary;
    }

    evmc::result call(const evmc_message& msg) noexcept final
    {
        (void)msg;
        // TODO: Improve C++ API for result creation.
        evmc_result res{};
        res.status_code = EVMC_FAILURE;
        return evmc::result{res};
    }

    evmc_tx_context get_tx_context() noexcept final { return {}; }

    evmc_bytes32 get_block_hash(int64_t number) noexcept final
    {
        int64_t current_block_number = get_tx_context().block_number;

        auto example_block_hash = evmc_bytes32{};
        if (number < current_block_number && number >= current_block_number - 256)
            example_block_hash = {{1, 1, 1, 1}};
        return example_block_hash;
    }

    void emit_log(const evmc_address& addr,
                  const uint8_t* data,
                  size_t data_size,
                  const evmc_bytes32 topics[],
                  size_t topics_count) noexcept final
    {
        (void)addr;
        (void)data;
        (void)data_size;
        (void)topics;
        (void)topics_count;
    }
};


extern "C" {

evmc_context* example_host_create_context()
{
    return new ExampleHost;
}

void example_host_destroy_context(evmc_context* context)
{
    delete static_cast<ExampleHost*>(context);
}
}
