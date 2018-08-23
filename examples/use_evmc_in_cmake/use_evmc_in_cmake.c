/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */

/** This example shows how to use evmc INTERFACE library from evmc CMake package. */

#include <evmc/evmc.h>

int main()
{
    struct evmc_instance instance = {.abi_version = EVMC_ABI_VERSION};
    return instance.abi_version - EVMC_ABI_VERSION;
}
