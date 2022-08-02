// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

/**
 * @file
 * The loader OS mock for opening DLLs. To be inserted in loader.c for unit tests.
 */

static const int magic_handle = 0xE7AC;

const char* evmc_test_library_path = NULL;
const char* evmc_test_library_symbol = NULL;
evmc_create_fn evmc_test_create_fn = NULL;

static const char* evmc_test_last_error_msg = NULL;

/* Limited variant of strcpy_s(). Exposed to unittests when building with EVMC_LOADER_MOCK. */
int strcpy_sx(char* dest, size_t destsz, const char* src);

static int evmc_test_load_library(const char* filename)
{
    evmc_test_last_error_msg = NULL;
    if (filename && evmc_test_library_path && strcmp(filename, evmc_test_library_path) == 0)
        return magic_handle;
    evmc_test_last_error_msg = "cannot load library";
    return 0;
}

static void evmc_test_free_library(int handle)
{
    (void)handle;
}

static evmc_create_fn evmc_test_get_symbol_address(int handle, const char* symbol)
{
    if (handle != magic_handle)
        return NULL;

    if (evmc_test_library_symbol && strcmp(symbol, evmc_test_library_symbol) == 0)
        return evmc_test_create_fn;
    return NULL;
}

static const char* evmc_test_get_last_error_msg(void)
{
    // Return the last error message only once.
    const char* m = evmc_test_last_error_msg;
    evmc_test_last_error_msg = NULL;
    return m;
}

#define DLL_HANDLE int
#define DLL_OPEN(filename) evmc_test_load_library(filename)
#define DLL_CLOSE(handle) evmc_test_free_library(handle)
#define DLL_GET_CREATE_FN(handle, name) evmc_test_get_symbol_address(handle, name)
#define DLL_GET_ERROR_MSG() evmc_test_get_last_error_msg()
