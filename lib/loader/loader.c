/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#include <evmc/loader.h>

#include <stdint.h>
#include <string.h>

#if _WIN32
#include <Windows.h>
#define DLL_HANDLE HMODULE
#define DLL_OPEN(filename) LoadLibrary(filename)
#define DLL_CLOSE(handle) FreeLibrary(handle)
#define DLL_GET_CREATE_FN(handle, name) (evmc_create_fn) GetProcAddress(handle, name)
#define HAVE_STRCPY_S 1
#else
#include <dlfcn.h>
#define DLL_HANDLE void*
#define DLL_OPEN(filename) dlopen(filename, RTLD_LAZY)
#define DLL_CLOSE(handle) dlclose(handle)
#define DLL_GET_CREATE_FN(handle, name) (evmc_create_fn)(uintptr_t) dlsym(handle, name)
#define HAVE_STRCPY_S 0
#endif

#define PATH_MAX_LENGTH 4096

#if !HAVE_STRCPY_S
static void strcpy_s(char* dest, size_t destsz, const char* src)
{
    size_t len = strlen(src);
    if (len > destsz - 1)
        len = destsz - 1;
    memcpy(dest, src, len);
    dest[len] = 0;
}
#endif

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

    DLL_HANDLE handle = DLL_OPEN(filename);
    if (!handle)
    {
        ec = EVMC_LOADER_CANNOT_OPEN;
        goto exit;
    }

    const char prefix[] = "evmc_create_";
    const size_t prefix_length = strlen(prefix);
    char name[sizeof(prefix) + PATH_MAX_LENGTH];
    strcpy_s(name, sizeof(name), prefix);

    const char* sep_pos = strrchr(filename, '/');
    const char* name_pos = sep_pos ? sep_pos + 1 : filename;

    const char lib_prefix[] = "lib";
    const size_t lib_prefix_length = strlen(lib_prefix);
    if (strncmp(name_pos, lib_prefix, lib_prefix_length) == 0)
        name_pos += lib_prefix_length;

    strcpy_s(name + prefix_length, PATH_MAX_LENGTH, name_pos);

    char* ext_pos = strrchr(name, '.');
    if (ext_pos)
        *ext_pos = 0;

    char* dash_pos = name;
    while ((dash_pos = strchr(dash_pos, '-')) != NULL)
        *dash_pos++ = '_';

    evmc_create_fn create_fn = DLL_GET_CREATE_FN(handle, name);
    if (!create_fn)
    {
        const char* short_name_pos = strrchr(name, '_');
        if (short_name_pos)
        {
            short_name_pos += 1;
            memmove(name + prefix_length, short_name_pos, strlen(short_name_pos) + 1);
            create_fn = DLL_GET_CREATE_FN(handle, name);
        }
    }

    if (create_fn)
    {
        instance = create_fn();
    }
    else
    {
        DLL_CLOSE(handle);
        ec = EVMC_LOADER_SYMBOL_NOT_FOUND;
    }

exit:
    if (error_code)
        *error_code = ec;
    return instance;
}
