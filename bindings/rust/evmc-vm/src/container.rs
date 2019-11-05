/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use crate::EvmcVm;

use std::ops::Deref;

/// Container struct for EVMC instances and user-defined data.
pub struct EvmcContainer<T>
where
    T: EvmcVm + Sized,
{
    #[allow(dead_code)]
    instance: ::evmc_sys::evmc_vm,
    vm: T,
}

impl<T> EvmcContainer<T>
where
    T: EvmcVm + Sized,
{
    /// Basic constructor.
    pub fn new(_instance: ::evmc_sys::evmc_vm) -> Box<Self> {
        Box::new(Self {
            instance: _instance,
            vm: T::init(),
        })
    }

    /// Take ownership of the given pointer and return a box.
    pub unsafe fn from_ffi_pointer(instance: *mut ::evmc_sys::evmc_vm) -> Box<Self> {
        assert!(!instance.is_null(), "from_ffi_pointer received NULL");
        Box::from_raw(instance as *mut EvmcContainer<T>)
    }

    /// Convert boxed self into an FFI pointer, surrendering ownership of the heap data.
    pub unsafe fn into_ffi_pointer(boxed: Box<Self>) -> *mut ::evmc_sys::evmc_vm {
        Box::into_raw(boxed) as *mut ::evmc_sys::evmc_vm
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
    use crate::types::*;
    use crate::{ExecutionContext, ExecutionMessage, ExecutionResult};

    struct TestVm {}

    impl EvmcVm for TestVm {
        fn init() -> Self {
            TestVm {}
        }
        fn execute(
            &self,
            _revision: evmc_sys::evmc_revision,
            _code: &[u8],
            _message: &ExecutionMessage,
            _context: Option<&mut ExecutionContext>,
        ) -> ExecutionResult {
            ExecutionResult::failure()
        }
    }

    unsafe extern "C" fn get_dummy_tx_context(
        _context: *mut evmc_sys::evmc_host_context,
    ) -> evmc_sys::evmc_tx_context {
        evmc_sys::evmc_tx_context {
            tx_gas_price: Uint256::default(),
            tx_origin: Address::default(),
            block_coinbase: Address::default(),
            block_number: 0,
            block_timestamp: 0,
            block_gas_limit: 0,
            block_difficulty: Uint256::default(),
            chain_id: Uint256::default(),
        }
    }

    #[test]
    fn container_new() {
        let instance = ::evmc_sys::evmc_vm {
            abi_version: ::evmc_sys::EVMC_ABI_VERSION as i32,
            name: std::ptr::null(),
            version: std::ptr::null(),
            destroy: None,
            execute: None,
            get_capabilities: None,
            set_option: None,
        };

        let code = [0u8; 0];

        let message = ::evmc_sys::evmc_message {
            kind: ::evmc_sys::evmc_call_kind::EVMC_CALL,
            flags: 0,
            depth: 0,
            gas: 0,
            destination: ::evmc_sys::evmc_address::default(),
            sender: ::evmc_sys::evmc_address::default(),
            input_data: std::ptr::null(),
            input_size: 0,
            value: ::evmc_sys::evmc_uint256be::default(),
            create2_salt: ::evmc_sys::evmc_bytes32::default(),
        };
        let message: ExecutionMessage = (&message).into();

        let host = ::evmc_sys::evmc_host_interface {
            account_exists: None,
            get_storage: None,
            set_storage: None,
            get_balance: None,
            get_code_size: None,
            get_code_hash: None,
            copy_code: None,
            selfdestruct: None,
            call: None,
            get_tx_context: Some(get_dummy_tx_context),
            get_block_hash: None,
            emit_log: None,
        };
        let host_context = std::ptr::null_mut();

        let mut context = ExecutionContext::new(&host, host_context);
        let container = EvmcContainer::<TestVm>::new(instance);
        assert_eq!(
            container
                .execute(
                    evmc_sys::evmc_revision::EVMC_PETERSBURG,
                    &code,
                    &message,
                    Some(&mut context)
                )
                .status_code(),
            ::evmc_sys::evmc_status_code::EVMC_FAILURE
        );

        let ptr = unsafe { EvmcContainer::into_ffi_pointer(container) };

        let mut context = ExecutionContext::new(&host, host_context);
        let container = unsafe { EvmcContainer::<TestVm>::from_ffi_pointer(ptr) };
        assert_eq!(
            container
                .execute(
                    evmc_sys::evmc_revision::EVMC_PETERSBURG,
                    &code,
                    &message,
                    Some(&mut context)
                )
                .status_code(),
            ::evmc_sys::evmc_status_code::EVMC_FAILURE
        );
    }
}
