/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

/** This example shows how to use evmc INTERFACE library from evmc CMake package. */

#include <evmc/evmc.h>

int main()
{
    struct evmc_instance instance = {.abi_version = EVMC_ABI_VERSION};
    return instance.abi_version - EVMC_ABI_VERSION;
}
