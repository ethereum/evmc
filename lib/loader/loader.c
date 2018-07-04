/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#include <evmc/loader.h>

#include <stdint.h>
#include <string.h>

#include <dlfcn.h>

#define PATH_MAX_LENGTH 4096

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

typedef struct evmc_instance* (*evmc_create_fn)();

struct evmc_instance* evmc_load(const char* filename, enum evmc_loader_error_code* error_code)
{
    enum evmc_loader_error_code ec = EVMC_LOADER_SUCCESS;
    struct evmc_instance* instance = NULL;

    if (!filename)
    {
        ec = EVMC_LOADER_INVALID_ARGUMENT;
        goto exit;
    }

    const size_t length = strlen(filename);
    if (length == 0 || length > PATH_MAX_LENGTH)
    {
        ec = EVMC_LOADER_INVALID_ARGUMENT;
        goto exit;
    }

    void* handle = dlopen(filename, RTLD_LAZY);
    if (!handle)
    {
        ec = EVMC_LOADER_CANNOT_OPEN;
        goto exit;
    }

    const char prefix[] = "evmc_create_";
    const size_t prefix_length = strlen(prefix);
    char name[sizeof(prefix) + PATH_MAX_LENGTH];
    strcpy(name, prefix);

    const char* sep_pos = strrchr(filename, '/');
    const char* name_pos = sep_pos ? sep_pos + 1 : filename;

    const char lib_prefix[] = "lib";
    const size_t lib_prefix_length = strlen(lib_prefix);
    if (strncmp(name_pos, lib_prefix, lib_prefix_length) == 0)
        name_pos += lib_prefix_length;

    strncpy(name + prefix_length, name_pos, PATH_MAX_LENGTH);

    char* ext_pos = strrchr(name, '.');
    if (ext_pos)
        *ext_pos = 0;

    char* dash_pos = name;
    while ((dash_pos = strchr(dash_pos, '-')) != NULL)
        *dash_pos++ = '_';

    const void* symbol = dlsym(handle, name);
    if (!symbol)
    {
        const char* short_name_pos = strrchr(name, '_');
        if (short_name_pos)
        {
            short_name_pos += 1;
            memmove(name + prefix_length, short_name_pos, strlen(short_name_pos) + 1);
            symbol = dlsym(handle, name);
        }
    }

    if (symbol)
    {
        evmc_create_fn create_fn = (evmc_create_fn)(uintptr_t)symbol;
        instance = create_fn();
    }
    else
    {
        dlclose(handle);
        ec = EVMC_LOADER_SYMBOL_NOT_FOUND;
    }

exit:
    if (error_code)
        *error_code = ec;
    return instance;
}
