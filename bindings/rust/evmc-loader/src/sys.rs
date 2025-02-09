// EVMC: Ethereum Client-VM Connector API.
// Copyright 2022 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

// Defining evmc_vm here, because bindgen cannot create a useful declaration yet.

pub type evmc_vm = ::std::os::raw::c_void;

#[cfg(test)]
mod tests {
    use std::ffi::CString;
    use std::os::raw::c_char;

    use super::*;

    #[test]
    fn load_fail() {
        let c_str = CString::new("test.so").unwrap();
        unsafe {
            let mut error_code = evmc_loader_error_code::EVMC_LOADER_UNSPECIFIED_ERROR;
            let instance = evmc_load_and_create(c_str.as_ptr() as *const c_char, &mut error_code);
            println!("{:?} {:?}", error_code, instance);
        }
    }
}
