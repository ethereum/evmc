/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#pragma once

#ifdef _MSC_VER
#define EVMC_EXPORT __declspec(dllexport)
#else
#define EVMC_EXPORT __attribute__ ((visibility ("default")))
#endif
