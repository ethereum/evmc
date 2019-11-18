/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018-2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

// Test compilation of C and C++ public headers.

#include <evmc/evmc.h>
#include <evmc/evmc.hpp>
#include <evmc/helpers.h>
#include <evmc/instructions.h>
#include <evmc/loader.h>
#include <evmc/mocked_host.hpp>
#include <evmc/utils.h>

// Include again to check if headers have proper include guards.
#include <evmc/evmc.h>
#include <evmc/evmc.hpp>
#include <evmc/helpers.h>
#include <evmc/instructions.h>
#include <evmc/loader.h>
#include <evmc/mocked_host.hpp>
#include <evmc/utils.h>
