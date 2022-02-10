// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

/** This example shows how to use evmc INTERFACE library from evmc CMake package. */

#include <evmc/evmc.h>

int main()
{
    struct evmc_vm vm = {.abi_version = EVMC_ABI_VERSION};
    return vm.abi_version - EVMC_ABI_VERSION;
}
