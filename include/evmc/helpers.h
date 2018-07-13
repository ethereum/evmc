/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

/**
 * EVMC Helpers
 *
 * A collection of helper functions for invoking a VM instance methods.
 * These are convenient for languages where invoking function pointers
 * is "ugly" or impossible (such as Go).
 *
 * @defgroup helpers EVMC Helpers
 * @{
 */
#pragma once

#include <evmc/evmc.h>

/**
 * Destroys the VM instance.
 *
 * @see evmc_destroy_fn
 */
static inline void evmc_destroy(struct evmc_instance* instance)
{
    instance->destroy(instance);
}

/**
 * Sets the option for the VM instance, if the feature is supported by the VM.
 *
 * @see evmc_set_option_fn
 */
static inline int evmc_set_option(struct evmc_instance* instance,
                                  char const* name,
                                  char const* value)
{
    if (instance->set_option)
        return instance->set_option(instance, name, value);
    return 0;
}

/**
 * Releases the resources allocated to the execution result.
 *
 * @see evmc_release_result_fn
 */
static inline void evmc_release_result(struct evmc_result* result)
{
    result->release(result);
}

/** @} */
