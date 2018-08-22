/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */

#include <evmc/evmc.h>
#include <evmc/utils.h>

EVMC_EXPORT void* evmc_create_aaa()
{
    return (void*)0xaaa;
}

EVMC_EXPORT void* evmc_create_eee_bbb()
{
    return (void*)0xeeebbb;
}

EVMC_EXPORT void* evmc_create_failure()
{
    return NULL;
}

EVMC_EXPORT struct evmc_instance* evmc_create_abi42()
{
    static struct evmc_instance instance = {
        .abi_version = 42,
    };
    return &instance;
}
