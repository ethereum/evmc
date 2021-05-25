// EVMC: Ethereum Client-VM Connector API.
// Copyright 2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/evmc.hpp>
#include <iosfwd>
#include <string>

namespace evmc::tooling
{
using vm_load_fn = int(evmc::VM&, const char* vm_config, std::ostream& err);

int main(int argc,
         const char** argv,
         const char* name,
         const char* version,
         vm_load_fn vm_load,
         evmc::VM default_vm);

int run(evmc::VM& vm,
        evmc_revision rev,
        int64_t gas,
        const std::string& code_hex,
        const std::string& input_hex,
        bool create,
        bool bench,
        std::ostream& out);
}  // namespace evmc::tooling
