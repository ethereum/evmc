/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

pub mod host;
mod loader;
pub mod types;
pub use self::loader::EvmcLoaderErrorCode;

use crate::loader::evmc_load_and_create;
use crate::types::*;
use evmc_sys as ffi;
use std::ffi::CStr;

extern "C" {
    fn evmc_create() -> *mut ffi::evmc_vm;
}

fn error(err: EvmcLoaderErrorCode) -> Result<EvmcLoaderErrorCode, &'static str> {
    match err {
        EvmcLoaderErrorCode::EvmcLoaderSucces => Ok(EvmcLoaderErrorCode::EvmcLoaderSucces),
        EvmcLoaderErrorCode::EvmcLoaderCannotOpen => Err("evmc loader: library cannot open"),
        EvmcLoaderErrorCode::EvmcLoaderSymbolNotFound => {
            Err("evmc loader: the EVMC create function not found")
        }
        EvmcLoaderErrorCode::EvmcLoaderInvalidArgument => {
            panic!("evmc loader: filename argument is invalid")
        }
        EvmcLoaderErrorCode::EvmcLoaderInstanceCreationFailure => {
            Err("evmc loader: VM instance creation failure")
        }
        EvmcLoaderErrorCode::EvmcLoaderAbiVersionMismatch => {
            Err("evmc loader: ABI version mismatch")
        }
        _ => Err("evmc loader: unexpected error"),
    }
}

pub struct EvmcVm {
    handle: *mut ffi::evmc_vm,
    host_context: *mut ffi::evmc_host_context,
    host_interface: *mut ffi::evmc_host_interface,
}

impl EvmcVm {
    pub fn get_abi_version(&self) -> i32 {
        unsafe {
            let version: i32 = (*self.handle).abi_version;
            version
        }
    }

    pub fn get_name(&self) -> &str {
        unsafe {
            let c_str: &CStr = CStr::from_ptr((*self.handle).name);
            c_str.to_str().unwrap()
        }
    }

    pub fn get_version(&self) -> &str {
        unsafe {
            let c_str: &CStr = CStr::from_ptr((*self.handle).version);
            c_str.to_str().unwrap()
        }
    }

    pub fn destroy(&self) {
        unsafe { ((*self.handle).destroy.unwrap())(self.handle) }
    }

    pub fn execute(
        &self,
        host_interface: Box<dyn host::HostInterface>,
        rev: Revision,
        kind: MessageKind,
        is_static: bool,
        depth: i32,
        gas: i64,
        destination: &Address,
        sender: &Address,
        input: &Bytes,
        value: &Bytes32,
        code: &Bytes,
        create2_salt: &Bytes32,
    ) -> (&Bytes, i64, StatusCode) {
        host::set_host_interface(Some(host_interface));
        let evmc_destination = ffi::evmc_address {
            bytes: *destination,
        };
        let evmc_sender = ffi::evmc_address { bytes: *sender };
        let evmc_value = ffi::evmc_uint256be { bytes: *value };
        let evmc_create2_salt = ffi::evmc_bytes32 {
            bytes: *create2_salt,
        };
        let mut evmc_flags: u32 = 0;
        unsafe {
            if is_static {
                evmc_flags |=
                    std::mem::transmute::<ffi::evmc_flags, u32>(ffi::evmc_flags::EVMC_STATIC);
            }
        }
        let evmc_message = Box::into_raw(Box::new({
            ffi::evmc_message {
                kind: kind,
                flags: evmc_flags,
                depth: depth,
                gas: gas,
                destination: evmc_destination,
                sender: evmc_sender,
                input_data: input.as_ptr(),
                input_size: input.len(),
                value: evmc_value,
                create2_salt: evmc_create2_salt,
            }
        }));

        unsafe {
            let result = ((*self.handle).execute.unwrap())(
                self.handle,
                self.host_interface,
                self.host_context,
                rev,
                evmc_message,
                code.as_ptr(),
                code.len(),
            );
            host::set_host_interface(None);
            return (
                std::slice::from_raw_parts(result.output_data, result.output_size),
                result.gas_left,
                result.status_code,
            );
        }
    }

    pub fn has_capability(&self, capability: Capabilities) -> bool {
        unsafe {
            std::mem::transmute::<Capabilities, u32>(capability)
                == ((*self.handle).get_capabilities.unwrap())(self.handle)
        }
    }
}

pub fn load(fname: &str) -> (EvmcVm, Result<EvmcLoaderErrorCode, &'static str>) {
    let (instance, ec) = evmc_load_and_create(fname);
    (
        EvmcVm {
            handle: instance,
            host_context: Box::into_raw(Box::new(host::get_evmc_host_context())),
            host_interface: Box::into_raw(Box::new(host::get_evmc_host_interface())),
        },
        error(ec),
    )
}

pub fn create() -> EvmcVm {
    unsafe {
        EvmcVm {
            handle: evmc_create(),
            host_context: Box::into_raw(Box::new(host::get_evmc_host_context())),
            host_interface: Box::into_raw(Box::new(host::get_evmc_host_interface())),
        }
    }
}
