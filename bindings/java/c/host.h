/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019-2020 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */
#include "evmc/evmc.h"
#include <jni.h>

#ifndef _Included_org_ethereum_evmc_Host
#define _Included_org_ethereum_evmc_Host
#ifdef __cplusplus
extern "C" {
#endif

struct evmc_host_context
{
    int index;
};

int evmc_java_set_jvm(JNIEnv*);
const struct evmc_host_interface* evmc_java_get_host_interface();

#ifdef __cplusplus
}
#endif
#endif
