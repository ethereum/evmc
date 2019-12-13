// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/evmc.hpp>
#include <iostream>

namespace evmc
{
int check_command(VM& vm)
{
    std::cout << "Checking " << vm.name() << " " << vm.version() << "\n";
    return 0;
}
}  // namespace evmc