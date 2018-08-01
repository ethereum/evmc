/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#pragma once

#if defined _MSC_VER || defined __MINGW32__
#define EVMC_EXPORT __declspec(dllexport)
#else
#define EVMC_EXPORT __attribute__((visibility("default")))
#endif

#if __cplusplus
#define EVMC_NOEXCEPT noexcept
#else
#define EVMC_NOEXCEPT
#endif
