/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use evmc_sys as ffi;
use libloading::{Library, Symbol};
use std::ptr;
use std::str;

type EvmcCreate = extern "C" fn() -> *mut ffi::evmc_vm;

#[derive(Debug)]
pub enum EvmcLoaderErrorCode {
    /** The loader succeeded. */
    EvmcLoaderSucces = 0,

    /** The loader cannot open the given file name. */
    EvmcLoaderCannotOpen = 1,

    /** The VM create function not found. */
    EvmcLoaderSymbolNotFound = 2,

    /** The invalid argument value provided. */
    EvmcLoaderInvalidArgument = 3,

    /** The creation of a VM instance has failed. */
    EvmcLoaderInstanceCreationFailure = 4,

    /** The ABI version of the VM instance has mismatched. */
    EvmcLoaderAbiVersionMismatch = 5,

    /** The VM option is invalid. */
    EvmcLoaderInvalidOptionName = 6,

    /** The VM option value is invalid. */
    EvmcLoaderInvalidOptionValue = 7,
}

pub fn evmc_load_and_create(fname: &str) -> (*mut ffi::evmc_vm, EvmcLoaderErrorCode) {
    unsafe {
        let mut instance: *mut ffi::evmc_vm = ptr::null_mut();

        let library: Library = match Library::new(fname) {
            Ok(lib) => lib,
            Err(_) => {
                return (instance, EvmcLoaderErrorCode::EvmcLoaderCannotOpen);
            }
        };

        let evmc_create_fn: Symbol<EvmcCreate> = match library.get(b"evmc_create\0") {
            Ok(symbol) => symbol,
            Err(_) => {
                return (instance, EvmcLoaderErrorCode::EvmcLoaderSymbolNotFound);
            }
        };

        instance = evmc_create_fn();

        if instance.is_null() {
            return (
                instance,
                EvmcLoaderErrorCode::EvmcLoaderInstanceCreationFailure,
            );
        }

        if (*instance).abi_version
            != std::mem::transmute::<ffi::_bindgen_ty_1, i32>(ffi::EVMC_ABI_VERSION)
        {
            return (instance, EvmcLoaderErrorCode::EvmcLoaderAbiVersionMismatch);
        }
        return (instance, EvmcLoaderErrorCode::EvmcLoaderSucces);
    }
}
