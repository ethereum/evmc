/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#pragma once

#include <evmc/evmc.h>

/**
 * Creates EVMC Example VM.
 */
struct evmc_instance* evmc_create_examplevm(void);
