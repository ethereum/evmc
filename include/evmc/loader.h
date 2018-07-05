/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 Pawel Bylica.
 * Licensed under the MIT License. See the LICENSE file.
 */

#pragma once

#if __cplusplus
extern "C" {
#endif

/** Error codes for the EVMC loader. */
enum evmc_loader_error_code
{
    EVMC_LOADER_SUCCESS = 0,
    EVMC_LOADER_CANNOT_OPEN,
    EVMC_LOADER_SYMBOL_NOT_FOUND,
    EVMC_LOADER_INVALID_ARGUMENT,
};

/**
 * Dynamically loads the shared object (DLL) with an EVM implementation.
 *
 * This function tries to open a DLL at the given `filename`. On UNIX-like systems dlopen() function
 * is used. On Windows LoadLibrary() function is used.
 *
 * If the file does not exist or is not a valid shared library the ::EVMC_ERRC_CANNOT_OPEN error
 * code is signaled and NULL is returned.
 *
 * After the DLL is successfully loaded the function tries to find the EVM create function in the
 * library. The `filename` is used to guess the EVM name and the name of the create function.
 * The create function name is constructed by the following rules. Consider example path:
 * "/ethereum/libexample-interpreter.so".
 * - the filename is taken from the path:
 *   "libexample-interpreter.so",
 * - the "lib" prefix and file extension are stripped from the name:
 *   "example-interpreter"
 * - all "-" are replaced with "_" to construct _full name_:
 *   "example_interpreter",
 * - the _full name_ is split by "_" char and the last item is taken to form the _short name_:
 *   "interpreter",
 * - the name "evmc_create_" + _full name_ is checked in the library:
 *   "evmc_create_example_interpreter",
 * - the name "evmc_create_" + _short name_ is checked in the library:
 *   "evmc_create_interpreter".
 *
 * If the create function is found in the library, the EVM instance is create and returned.
 * Otherwise, the ::EVMC_ERRC_SYMBOL_NOT_FOUND error code is signaled and NULL is returned.
 *
 * @param filename    The null terminated path (absolute or relative) to the shared library
 *                    containing the EVM implementation. If the value is NULL, an empty C-string
 *                    or longer than the path maximum length the ::EVMC_ERRC_INVALID_ARGUMENT is
 *                    signaled.
 * @param error_code  The pointer to the error code. If not NULL the value is set to
 *                    ::EVMC_ERRC_SUCCESS on success or any other error code as described above.
 * @return            The pointer to EVM instance if loaded successfully or NULL.
 */
struct evmc_instance* evmc_load(const char* filename, enum evmc_loader_error_code* error_code);

#if __cplusplus
}
#endif
