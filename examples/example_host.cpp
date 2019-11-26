/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2016-2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

/// @file
/// Example implementation of an EVMC Host.

#include "example_host.h"

#include <evmc/evmc.hpp>

#include <algorithm>
#include <map>
#include <vector>

using namespace evmc::literals;

namespace evmc
{
struct account
{
    evmc::uint256be balance = {};
    std::vector<uint8_t> code;
    std::map<evmc::bytes32, evmc::bytes32> storage;

    virtual evmc::bytes32 code_hash() const
    {
        // Extremely dumb "hash" function.
        evmc::bytes32 ret{};
        for (std::vector<uint8_t>::size_type i = 0; i != code.size(); i++)
        {
            auto v = code[i];
            ret.bytes[v % sizeof(ret.bytes)] ^= v;
        }
        return ret;
    }
};

using accounts = std::map<evmc::address, account>;

}  // namespace evmc

class ExampleHost : public evmc::Host
{
    evmc::accounts accounts;
    evmc_tx_context tx_context{};

public:
    ExampleHost() = default;
    explicit ExampleHost(evmc_tx_context& _tx_context) noexcept : tx_context{_tx_context} {};
    ExampleHost(evmc_tx_context& _tx_context, evmc::accounts& _accounts) noexcept
      : accounts{_accounts}, tx_context{_tx_context} {};

    bool account_exists(const evmc::address& addr) const noexcept final
    {
        return accounts.find(addr) != accounts.end();
    }

    evmc::bytes32 get_storage(const evmc::address& addr, const evmc::bytes32& key) const
        noexcept final
    {
        const auto account_iter = accounts.find(addr);
        if (account_iter == accounts.end())
            return {};

        const auto storage_iter = account_iter->second.storage.find(key);
        if (storage_iter != account_iter->second.storage.end())
            return storage_iter->second;
        return {};
    }

    evmc_storage_status set_storage(const evmc::address& addr,
                                    const evmc::bytes32& key,
                                    const evmc::bytes32& value) noexcept final
    {
        auto& account = accounts[addr];
        auto prev_value = account.storage[key];
        account.storage[key] = value;

        return (prev_value == value) ? EVMC_STORAGE_UNCHANGED : EVMC_STORAGE_MODIFIED;
    }

    evmc::uint256be get_balance(const evmc::address& addr) const noexcept final
    {
        auto it = accounts.find(addr);
        if (it != accounts.end())
            return it->second.balance;
        return {};
    }

    size_t get_code_size(const evmc::address& addr) const noexcept final
    {
        auto it = accounts.find(addr);
        if (it != accounts.end())
            return it->second.code.size();
        return 0;
    }

    evmc::bytes32 get_code_hash(const evmc::address& addr) const noexcept final
    {
        auto it = accounts.find(addr);
        if (it != accounts.end())
            return it->second.code_hash();
        return {};
    }

    size_t copy_code(const evmc::address& addr,
                     size_t code_offset,
                     uint8_t* buffer_data,
                     size_t buffer_size) const noexcept final
    {
        const auto it = accounts.find(addr);
        if (it == accounts.end())
            return 0;

        const auto& code = it->second.code;

        if (code_offset >= code.size())
            return 0;

        const auto n = std::min(buffer_size, code.size() - code_offset);

        if (n > 0)
            std::copy_n(&code[code_offset], n, buffer_data);
        return n;
    }

    void selfdestruct(const evmc::address& addr, const evmc::address& beneficiary) noexcept final
    {
        (void)addr;
        (void)beneficiary;
    }

    evmc::result call(const evmc_message& msg) noexcept final
    {
        return {EVMC_REVERT, msg.gas, msg.input_data, msg.input_size};
    }

    evmc_tx_context get_tx_context() const noexcept final { return tx_context; }

    evmc::bytes32 get_block_hash(int64_t number) const noexcept final
    {
        const int64_t current_block_number = get_tx_context().block_number;

        return (number < current_block_number && number >= current_block_number - 256) ?
                   0xb10c8a5fb10c8a5fb10c8a5fb10c8a5fb10c8a5fb10c8a5fb10c8a5fb10c8a5f_bytes32 :
                   0_bytes32;
    }

    void emit_log(const evmc::address& addr,
                  const uint8_t* data,
                  size_t data_size,
                  const evmc::bytes32 topics[],
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

const evmc_host_interface* example_host_get_interface()
{
    return &evmc::Host::get_interface();
}

evmc_host_context* example_host_create_context(evmc_tx_context tx_context)
{
    auto host = new ExampleHost{tx_context};
    return host->to_context();
}

void example_host_destroy_context(evmc_host_context* context)
{
    delete evmc::Host::from_context<ExampleHost>(context);
}
}
