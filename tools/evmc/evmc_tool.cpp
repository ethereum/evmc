// EVMC: Ethereum Client-VM Connector API
// Copyright 2021 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/loader.h>
#include <evmc/tooling.hpp>

int main(int argc, const char* argv[])
{
    return evmc::tooling::main(argc, argv, "EVMC", PROJECT_VERSION,
                               [](evmc::VM& vm, const char* vm_config, std::ostream& err) {
                                   evmc_loader_error_code ec;
                                   vm = evmc::VM{evmc_load_and_configure(vm_config, &ec)};
                                   if (ec != EVMC_LOADER_SUCCESS)
                                   {
                                       const auto error = evmc_last_error_msg();
                                       if (error != nullptr)
                                           err << error << "\n";
                                       else
                                           err << "Loading error " << ec << "\n";
                                       return static_cast<int>(ec);
                                   }
                                   return 0;
                               },
                               {});
}
