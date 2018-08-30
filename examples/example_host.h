/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */

#include <evmc/evmc.h>

#if __cplusplus
extern "C" {
#endif

struct evmc_context* example_host_create_context();

void example_host_destroy_context(struct evmc_context* context);

#if __cplusplus
}
#endif
