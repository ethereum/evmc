// EVMC: Ethereum Client-VM Connector API.
// Copyright 2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/evmc.hpp>
#include <iosfwd>
#include <string>

namespace evmc
{
namespace cmd
{
int run(evmc::VM& vm,
        evmc_revision rev,
        int64_t gas,
        const std::string& code_hex,
        const std::string& input_hex,
        bool create,
        std::ostream& out);
}
}  // namespace evmc
