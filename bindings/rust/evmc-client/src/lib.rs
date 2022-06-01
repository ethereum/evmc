/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#[macro_use]
extern crate enum_primitive;
pub mod host;
pub mod types;
use crate::types::*;
use evmc_loader;
use evmc_sys as ffi;
use std::ffi::CStr;

pub struct EvmcVm {
    handle: *mut ffi::evmc_vm,
    host_interface: *mut ffi::evmc_host_interface,
}

impl Drop for EvmcVm {
    fn drop(&mut self) {
        debug_assert!(!self.handle.is_null());
        unsafe { ((*self.handle).destroy.unwrap())(self.handle) }
    }
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

    pub fn execute(
        &self,
        ctx: &mut dyn host::HostContext,
        rev: Revision,
        kind: MessageKind,
        is_static: bool,
        depth: i32,
        gas: i64,
        recipient: &Address,
        sender: &Address,
        input: &Bytes,
        value: &Bytes32,
        code: &Bytes,
        create2_salt: &Bytes32,
        code_address: &Address,
    ) -> (&Bytes, i64, StatusCode) {
        let ext_ctx = host::ExtendedContext { hctx: ctx };
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
                recipient: ffi::evmc_address { bytes: *recipient },
                sender: ffi::evmc_address { bytes: *sender },
                input_data: input.as_ptr(),
                input_size: input.len(),
                value: ffi::evmc_uint256be { bytes: *value },
                create2_salt: ffi::evmc_bytes32 {
                    bytes: *create2_salt,
                },
                code_address: ffi::evmc_address {
                    bytes: *code_address,
                },
            }
        }));
        unsafe {
            let result = ((*self.handle).execute.unwrap())(
                self.handle,
                self.host_interface,
                // ext_ctx as *mut ffi::evmc_host_context,
                std::mem::transmute::<&host::ExtendedContext, *mut ffi::evmc_host_context>(
                    &ext_ctx,
                ),
                rev,
                evmc_message,
                code.as_ptr(),
                code.len(),
            );
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

pub fn load_and_configure(
    config: &str,
) -> Result<EvmcVm, (evmc_loader::sys::evmc_loader_error_code, String)> {
    Ok(EvmcVm {
        handle: evmc_loader::load_and_configure(config)? as *mut ffi::evmc_vm,
        host_interface: Box::into_raw(Box::new(host::get_evmc_host_interface())),
    })
}
