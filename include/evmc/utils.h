/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#pragma once

#if __cplusplus
#define EVMC_EXTERNC extern "C"
#else
#define EVMC_EXTERNC
#endif

#if _MSC_VER
#define EVMC_ENTRYPOINT(function_name)
#else
#define EVMC_ENTRYPOINT(function_name) \
    EVMC_EXTERNC struct evmc_instance* evmc_create() __attribute__((weak, alias(#function_name)));
#endif
