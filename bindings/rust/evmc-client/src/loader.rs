/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use evmc_sys as ffi;
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::str;
extern crate num;
use num::FromPrimitive;

#[link(name = "evmc-loader")]
extern "C" {
    fn evmc_load_and_create(
        filename: *const c_char,
        evmc_loader_error_code: *mut i32,
    ) -> *mut ffi::evmc_vm;
    fn evmc_last_error_msg() -> *const c_char;
}

enum_from_primitive! {
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
}

fn error(err: EvmcLoaderErrorCode) -> Result<EvmcLoaderErrorCode, &'static str> {
    match err {
        EvmcLoaderErrorCode::EvmcLoaderSucces => Ok(EvmcLoaderErrorCode::EvmcLoaderSucces),
        _ => unsafe { Err(CStr::from_ptr(evmc_last_error_msg()).to_str().unwrap()) },
    }
}

pub fn load_and_create(
    fname: &str,
) -> (*mut ffi::evmc_vm, Result<EvmcLoaderErrorCode, &'static str>) {
    let c_str = CString::new(fname).unwrap();
    unsafe {
        let mut error_code: i32 = 0;
        let instance = evmc_load_and_create(c_str.as_ptr() as *const c_char, &mut error_code);
        return (
            instance,
            error(EvmcLoaderErrorCode::from_i32(error_code).unwrap()),
        );
    }
}
