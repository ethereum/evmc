/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use crate::EvmcVm;
use crate::ExecutionContext;
use crate::ExecutionResult;

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
    pub unsafe fn into_ffi_pointer(boxed: Box<Self>) -> *const ::evmc_sys::evmc_instance {
        Box::into_raw(boxed) as *const ::evmc_sys::evmc_instance
    }

    // TODO: Maybe this can just be done with the Deref<Target = T> trait.
    pub fn execute(&self, code: &[u8], context: &ExecutionContext) -> ExecutionResult {
        self.vm.execute(code, context)
    }
}
