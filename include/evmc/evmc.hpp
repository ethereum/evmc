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

    vm(evmc_instance* instance, std::initializer_list<std::pair<const char*, const char*>> options)
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
}  // namespace evmc
