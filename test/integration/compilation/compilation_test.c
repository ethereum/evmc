/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018-2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

/* Test compilation of C public headers. */

#include <evmc/evmc.h>
#include <evmc/instructions.h>
#include <evmc/loader.h>
#include <evmc/utils.h>

#if _MSC_VER || __STDC_VERSION__ >= 199901
/* Skip C90 standard because contains inline keyword. */
#include <evmc/helpers.h>
#endif
