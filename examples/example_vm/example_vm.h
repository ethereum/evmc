/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018-2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#pragma once

#include <evmc/evmc.h>
#include <evmc/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates EVMC Example VM.
 */
EVMC_EXPORT struct evmc_vm* evmc_create_example_vm(void);

#ifdef __cplusplus
}
#endif
