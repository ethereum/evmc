// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

// Test compilation of C and C++ public headers.

#include <evmc/evmc.h>
#include <evmc/evmc.hpp>
#include <evmc/filter_iterator.hpp>
#include <evmc/helpers.h>
#include <evmc/hex.hpp>
#include <evmc/instructions.h>
#include <evmc/loader.h>
#include <evmc/mocked_host.hpp>
#include <evmc/utils.h>

// Include again to check if headers have proper include guards.
#include <evmc/evmc.h>               //NOLINT(readability-duplicate-include)
#include <evmc/evmc.hpp>             //NOLINT(readability-duplicate-include)
#include <evmc/filter_iterator.hpp>  //NOLINT(readability-duplicate-include)
#include <evmc/helpers.h>            //NOLINT(readability-duplicate-include)
#include <evmc/hex.hpp>              //NOLINT(readability-duplicate-include)
#include <evmc/instructions.h>       //NOLINT(readability-duplicate-include)
#include <evmc/loader.h>             //NOLINT(readability-duplicate-include)
#include <evmc/mocked_host.hpp>      //NOLINT(readability-duplicate-include)
#include <evmc/utils.h>              //NOLINT(readability-duplicate-include)
