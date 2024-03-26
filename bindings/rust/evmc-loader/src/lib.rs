// EVMC: Ethereum Client-VM Connector API.
// Copyright 2022 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

use std::ffi::{CStr, CString};

mod sys;

pub fn load_and_configure(
    config: &str,
) -> Result<*mut sys::evmc_vm, (sys::evmc_loader_error_code, String)> {
    let config_cstr = CString::new(config).unwrap();
    let mut error_code = sys::evmc_loader_error_code::EVMC_LOADER_UNSPECIFIED_ERROR;
    let instance = unsafe { sys::evmc_load_and_configure(config_cstr.as_ptr(), &mut error_code) };

    if error_code == sys::evmc_loader_error_code::EVMC_LOADER_SUCCESS {
        assert!(!instance.is_null());
        Ok(instance)
    } else {
        assert!(instance.is_null());
        let error_msg = unsafe { CStr::from_ptr(sys::evmc_last_error_msg()) }
            .to_str()
            .expect("well formed error message") // TODO free the vm
            .to_string();
        Err((error_code, error_msg))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn load_fail() {
        println!("{:?}", load_and_configure("test.so"));
    }
}
