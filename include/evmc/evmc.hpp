/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

/// @file
/// EVMC C++ API - wrappers and bindings for C++.

#include <evmc/evmc.h>
#include <evmc/helpers.h>

#include <initializer_list>
#include <utility>

namespace evmc
{
class result : public evmc_result
{
public:
    explicit result(evmc_result const& res) noexcept : evmc_result{res} {}

    ~result() noexcept
    {
        if (release)
            release(this);
    }

    result(result&& other) noexcept : evmc_result{other}
    {
        // Disable releaser of the rvalue object.
        other.release = nullptr;
    }

    result(result const&) = delete;

    result& operator=(result other) noexcept
    {
        std::swap(*this, other);
        return *this;
    }
};

class vm
{
public:
    explicit vm(evmc_instance* instance) noexcept : m_instance{instance} {}
    ~vm() noexcept { m_instance->destroy(m_instance); }

    vm(evmc_instance* instance,
       std::initializer_list<std::pair<const char*, const char*>> options) noexcept
      : m_instance{instance}
    {
        for (auto option : options)
            set_option(option.first, option.second);
    }

    bool is_abi_compatible() const noexcept { return m_instance->abi_version == EVMC_ABI_VERSION; }

    char const* name() const noexcept { return m_instance->name; }

    char const* version() const noexcept { return m_instance->version; }

    evmc_set_option_result set_option(const char name[], const char value[]) noexcept
    {
        return evmc_set_option(m_instance, name, value);
    }

    result execute(evmc_context& ctx,
                   evmc_revision rev,
                   const evmc_message& msg,
                   const uint8_t* code,
                   size_t code_size) noexcept
    {
        return result{m_instance->execute(m_instance, &ctx, rev, &msg, code, code_size)};
    }

private:
    evmc_instance* const m_instance = nullptr;
};


class HostInterface
{
public:
    virtual ~HostInterface() noexcept = default;

    virtual bool account_exists(const evmc_address& addr) noexcept = 0;

    virtual evmc_bytes32 get_storage(const evmc_address& addr,
                                     const evmc_bytes32& key) noexcept = 0;

    virtual evmc_storage_status set_storage(const evmc_address& addr,
                                            const evmc_bytes32& key,
                                            const evmc_bytes32& value) noexcept = 0;

    virtual evmc_uint256be get_balance(const evmc_address& addr) noexcept = 0;

    virtual size_t get_code_size(const evmc_address& addr) noexcept = 0;

    virtual evmc_bytes32 get_code_hash(const evmc_address& addr) noexcept = 0;

    virtual size_t copy_code(const evmc_address& addr,
                             size_t code_offset,
                             uint8_t* buffer_data,
                             size_t buffer_size) noexcept = 0;

    virtual void selfdestruct(const evmc_address& addr,
                              const evmc_address& beneficiary) noexcept = 0;

    virtual result call(const evmc_message& msg) noexcept = 0;

    virtual evmc_tx_context get_tx_context() noexcept = 0;

    virtual evmc_bytes32 get_block_hash(int64_t block_number) noexcept = 0;

    virtual void emit_log(const evmc_address& addr,
                          const uint8_t* data,
                          size_t data_size,
                          const evmc_bytes32 topics[],
                          size_t num_topics) noexcept = 0;
};


/// Wrapper around EVMC host context / host interface.
///
/// To be used by VM implementations as better alternative to using ::evmc_context directly.
class HostContext : public HostInterface
{
    evmc_context* context = nullptr;
    evmc_tx_context tx_context = {};

public:
    HostContext(evmc_context* context) noexcept : context{context} {}  // NOLINT

    bool account_exists(const evmc_address& address) noexcept final
    {
        return context->host->account_exists(context, &address);
    }

    evmc_bytes32 get_storage(const evmc_address& address, const evmc_bytes32& key) noexcept final
    {
        return context->host->get_storage(context, &address, &key);
    }

    evmc_storage_status set_storage(const evmc_address& address,
                                    const evmc_bytes32& key,
                                    const evmc_bytes32& value) noexcept final
    {
        return context->host->set_storage(context, &address, &key, &value);
    }

    evmc_uint256be get_balance(const evmc_address& address) noexcept final
    {
        return context->host->get_balance(context, &address);
    }

    size_t get_code_size(const evmc_address& address) noexcept final
    {
        return context->host->get_code_size(context, &address);
    }

    evmc_bytes32 get_code_hash(const evmc_address& address) noexcept final
    {
        return context->host->get_code_hash(context, &address);
    }

    size_t copy_code(const evmc_address& address,
                     size_t code_offset,
                     uint8_t* buffer_data,
                     size_t buffer_size) noexcept final
    {
        return context->host->copy_code(context, &address, code_offset, buffer_data, buffer_size);
    }

    void selfdestruct(const evmc_address& address, const evmc_address& beneficiary) noexcept final
    {
        context->host->selfdestruct(context, &address, &beneficiary);
    }

    result call(const evmc_message& message) noexcept final
    {
        return result{context->host->call(context, &message)};
    }

    /// Gets the transaction and block context from the Host.
    ///
    /// The implementation caches the received transaction context
    /// by assuming that the block timestamp should never be zero.
    ///
    /// @return The cached transaction context.
    evmc_tx_context get_tx_context() noexcept final
    {
        if (tx_context.block_timestamp == 0)
            tx_context = context->host->get_tx_context(context);
        return tx_context;
    }

    evmc_bytes32 get_block_hash(int64_t number) noexcept final
    {
        return context->host->get_block_hash(context, number);
    }

    void emit_log(const evmc_address& address,
                  const uint8_t* data,
                  size_t data_size,
                  const evmc_bytes32 topics[],
                  size_t topics_count) noexcept final
    {
        context->host->emit_log(context, &address, data, data_size, topics, topics_count);
    }
};
}  // namespace evmc
