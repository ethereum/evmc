/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use crate::EvmcVm;

use std::ops::Deref;

/// Container struct for EVMC instances and user-defined data.
pub struct EvmcContainer<T: EvmcVm + Sized> {
    #[allow(dead_code)]
    instance: ::evmc_sys::evmc_instance,
    vm: T,
}

impl<T: EvmcVm + Sized> EvmcContainer<T> {
    /// Basic constructor.
    pub fn new(_instance: ::evmc_sys::evmc_instance) -> Self {
        Self {
            instance: _instance,
            vm: T::init(),
        }
    }

    /// Take ownership of the given pointer and return a box.
    pub unsafe fn from_ffi_pointer(instance: *mut ::evmc_sys::evmc_instance) -> Box<Self> {
        assert!(!instance.is_null(), "from_ffi_pointer received NULL");
        Box::from_raw(instance as *mut EvmcContainer<T>)
    }

    /// Convert boxed self into an FFI pointer, surrendering ownership of the heap data.
    pub unsafe fn into_ffi_pointer(boxed: Box<Self>) -> *mut ::evmc_sys::evmc_instance {
        Box::into_raw(boxed) as *mut ::evmc_sys::evmc_instance
    }
}

impl<T> Deref for EvmcContainer<T>
where
    T: EvmcVm,
{
    type Target = T;

    fn deref(&self) -> &Self::Target {
        &self.vm
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{ExecutionContext, ExecutionResult};

    struct TestVm {}
    impl EvmcVm for TestVm {
        fn init() -> Self {
            TestVm {}
        }
        fn execute(&self, _code: &[u8], _context: &ExecutionContext) -> Result<ExecutionResult, ()> {
            Ok(ExecutionResult::failure())
        }
    }

    #[test]
    fn container_new() {
        let instance = ::evmc_sys::evmc_instance {
            abi_version: ::evmc_sys::EVMC_ABI_VERSION as i32,
            name: std::ptr::null(),
            version: std::ptr::null(),
            destroy: None,
            execute: None,
            get_capabilities: None,
            set_tracer: None,
            set_option: None,
        };

        let container = EvmcContainer::<TestVm>::new(instance);

        let ptr = unsafe { EvmcContainer::into_ffi_pointer(Box::new(container)) };

        unsafe { EvmcContainer::<TestVm>::from_ffi_pointer(ptr) };
    }
}
