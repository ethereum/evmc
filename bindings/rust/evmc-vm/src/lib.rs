/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

pub extern crate evmc_sys;

#[macro_use]
extern crate paste;

pub use evmc_sys as ffi;

// TODO: Add convenient helpers for evmc_execute
// TODO: Add a derive macro here for creating evmc_create

/// EVMC result structure.
pub struct ExecutionResult {
    status_code: ffi::evmc_status_code,
    gas_left: i64,
    output: Option<Vec<u8>>,
    create_address: ffi::evmc_address,
}

/// EVMC context structure. Exposes the EVMC host functions, message data, and transaction context
/// to the executing VM.
pub struct ExecutionContext<'a> {
    message: &'a ffi::evmc_message,
    context: &'a mut ffi::evmc_context,
    tx_context: ffi::evmc_tx_context,
}

impl ExecutionResult {
    pub fn new(
        _status_code: ffi::evmc_status_code,
        _gas_left: i64,
        _output: Option<Vec<u8>>,
        _create_address: Option<ffi::evmc_address>,
    ) -> Self {
        ExecutionResult {
            status_code: _status_code,
            gas_left: _gas_left,
            output: _output,
            create_address: {
                if let Some(_create_address) = _create_address {
                    _create_address
                } else {
                    ffi::evmc_address { bytes: [0u8; 20] }
                }
            },
        }
    }

    pub fn get_status_code(&self) -> ffi::evmc_status_code {
        self.status_code
    }

    pub fn get_gas_left(&self) -> i64 {
        self.gas_left
    }

    pub fn get_output(&self) -> Option<&Vec<u8>> {
        self.output.as_ref()
    }

    pub fn get_create_address(&self) -> Option<&ffi::evmc_address> {
        // Only return Some if the address is valid (e.g. the status is EVMC_SUCCESS)
        if self.status_code == ffi::evmc_status_code::EVMC_SUCCESS {
            Some(&self.create_address)
        } else {
            None
        }
    }
}

impl<'a> ExecutionContext<'a> {
    pub fn new(_message: &'a ffi::evmc_message, _context: &'a mut ffi::evmc_context) -> Self {
        let _tx_context = unsafe {
            assert!((*(_context.host)).get_tx_context.is_some());
            (*(_context.host)).get_tx_context.unwrap()(_context as *mut ffi::evmc_context)
        };

        ExecutionContext {
            message: _message,
            context: _context,
            tx_context: _tx_context,
        }
    }

    pub fn get_message(&self) -> &ffi::evmc_message {
        &self.message
    }

    pub fn get_tx_context(&mut self) -> &ffi::evmc_tx_context {
        &self.tx_context
    }

    pub fn account_exists(&mut self, address: &ffi::evmc_address) -> bool {
        unsafe {
            assert!((*self.context.host).account_exists.is_some());
            (*self.context.host).account_exists.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
            )
        }
    }

    pub fn get_storage(
        &mut self,
        address: &ffi::evmc_address,
        key: &ffi::evmc_bytes32,
    ) -> ffi::evmc_bytes32 {
        unsafe {
            assert!((*self.context.host).get_storage.is_some());
            (*self.context.host).get_storage.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
                key as *const ffi::evmc_bytes32,
            )
        }
    }

    pub fn set_storage(
        &mut self,
        address: &ffi::evmc_address,
        key: &ffi::evmc_bytes32,
        value: &ffi::evmc_bytes32,
    ) -> ffi::evmc_storage_status {
        unsafe {
            assert!((*self.context.host).set_storage.is_some());
            (*self.context.host).set_storage.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
                key as *const ffi::evmc_bytes32,
                value as *const ffi::evmc_bytes32,
            )
        }
    }

    pub fn get_balance(&mut self, address: &ffi::evmc_address) -> ffi::evmc_bytes32 {
        unsafe {
            assert!((*self.context.host).get_balance.is_some());
            (*self.context.host).get_balance.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
            )
        }
    }

    pub fn get_code_size(&mut self, address: &ffi::evmc_address) -> usize {
        unsafe {
            assert!((*self.context.host).get_code_size.is_some());
            (*self.context.host).get_code_size.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
            )
        }
    }

    pub fn get_code_hash(&mut self, address: &ffi::evmc_address) -> ffi::evmc_bytes32 {
        unsafe {
            assert!((*self.context.host).get_code_size.is_some());
            (*self.context.host).get_code_hash.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
            )
        }
    }

    pub fn copy_code(
        &mut self,
        address: &ffi::evmc_address,
        code_offset: usize,
        buffer: &mut [u8],
    ) -> usize {
        unsafe {
            assert!((*self.context.host).copy_code.is_some());
            (*self.context.host).copy_code.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
                code_offset,
                // FIXME: ensure that alignment of the array elements is OK
                buffer.as_mut_ptr(),
                buffer.len(),
            )
        }
    }

    pub fn selfdestruct(&mut self, address: &ffi::evmc_address, beneficiary: &ffi::evmc_address) {
        unsafe {
            assert!((*self.context.host).selfdestruct.is_some());
            (*self.context.host).selfdestruct.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
                beneficiary as *const ffi::evmc_address,
            )
        }
    }

    pub fn call(&mut self, message: &ffi::evmc_message) -> ExecutionResult {
        unsafe {
            assert!((*self.context.host).call.is_some());
            (*self.context.host).call.unwrap()(
                self.context as *mut ffi::evmc_context,
                message as *const ffi::evmc_message,
            )
            .into()
        }
    }

    pub fn get_block_hash(&mut self, num: i64) -> ffi::evmc_bytes32 {
        unsafe {
            assert!((*self.context.host).get_block_hash.is_some());
            (*self.context.host).get_block_hash.unwrap()(
                self.context as *mut ffi::evmc_context,
                num,
            )
        }
    }

    pub fn emit_log(
        &mut self,
        address: &ffi::evmc_address,
        data: &[u8],
        topics: &[ffi::evmc_bytes32],
    ) {
        unsafe {
            assert!((*self.context.host).emit_log.is_some());
            (*self.context.host).emit_log.unwrap()(
                self.context as *mut ffi::evmc_context,
                address as *const ffi::evmc_address,
                // FIXME: ensure that alignment of the array elements is OK
                data.as_ptr(),
                data.len(),
                topics.as_ptr(),
                topics.len(),
            )
        }
    }
}

impl From<ffi::evmc_result> for ExecutionResult {
    fn from(result: ffi::evmc_result) -> Self {
        let ret = ExecutionResult {
            status_code: result.status_code,
            gas_left: result.gas_left,
            output: if !result.output_data.is_null() {
                // Pre-allocate a vector.
                let mut buf: Vec<u8> = Vec::with_capacity(result.output_size);

                unsafe {
                    // Set the len of the vec manually.
                    buf.set_len(result.output_size);
                    // Copy from the C struct's buffer to the vec's buffer.
                    std::ptr::copy(result.output_data, buf.as_mut_ptr(), result.output_size);
                }

                Some(buf)
            } else {
                None
            },
            create_address: result.create_address,
        };

        // Release allocated ffi struct.
        if result.release.is_some() {
            unsafe {
                result.release.unwrap()(&result as *const ffi::evmc_result);
            }
        }

        ret
    }
}

fn allocate_output_data(output: Option<Vec<u8>>) -> (*const u8, usize) {
    if let Some(buf) = output {
        let buf_len = buf.len();

        // Manually allocate heap memory for the new home of the output buffer.
        let memlayout = std::alloc::Layout::from_size_align(buf_len, 1).expect("Bad layout");
        let new_buf = unsafe { std::alloc::alloc(memlayout) };
        unsafe {
            // Copy the data into the allocated buffer.
            std::ptr::copy(buf.as_ptr(), new_buf, buf_len);
        }

        (new_buf as *const u8, buf_len)
    } else {
        (std::ptr::null(), 0)
    }
}

unsafe fn deallocate_output_data(ptr: *const u8, size: usize) {
    if !ptr.is_null() {
        let buf_layout = std::alloc::Layout::from_size_align(size, 1).expect("Bad layout");
        std::alloc::dealloc(ptr as *mut u8, buf_layout);
    }
}

/// Returns a pointer to a heap-allocated evmc_result.
impl Into<*const ffi::evmc_result> for ExecutionResult {
    fn into(self) -> *const ffi::evmc_result {
        let (buffer, len) = allocate_output_data(self.output);
        Box::into_raw(Box::new(ffi::evmc_result {
            status_code: self.status_code,
            gas_left: self.gas_left,
            output_data: buffer,
            output_size: len,
            release: Some(release_heap_result),
            create_address: self.create_address,
            padding: [0u8; 4],
        }))
    }
}

/// Callback to pass across FFI, de-allocating the optional output_data.
extern "C" fn release_heap_result(result: *const ffi::evmc_result) {
    unsafe {
        let tmp = Box::from_raw(result as *mut ffi::evmc_result);
        deallocate_output_data(tmp.output_data, tmp.output_size);
    }
}

/// Returns a pointer to a stack-allocated evmc_result.
impl Into<ffi::evmc_result> for ExecutionResult {
    fn into(self) -> ffi::evmc_result {
        let (buffer, len) = allocate_output_data(self.output);
        ffi::evmc_result {
            status_code: self.status_code,
            gas_left: self.gas_left,
            output_data: buffer,
            output_size: len,
            release: Some(release_stack_result),
            create_address: self.create_address,
            padding: [0u8; 4],
        }
    }
}

/// Callback to pass across FFI, de-allocating the optional output_data.
extern "C" fn release_stack_result(result: *const ffi::evmc_result) {
    unsafe {
        let tmp = *result;
        deallocate_output_data(tmp.output_data, tmp.output_size);
    }
}

/*
struct MyVM {}

impl EVMCInstance for MyVM {
  fn execute(...) {
  }
}

evmc_create_vm!("myvm", "1.0", MyVM)

*/

//pub struct EvmcInstance(Box<ffi::evmc_instance>);
/*
pub struct EvmcInstance {
    instance: ffi::evmc_instance,
    executor: VMInstance
}

pub trait VMInstance {
  fn execute(instance: EvmcInstance, host: ffi::evmc_context, revision: ffi::evmc_revision, msg: ffi::evmc_message, code: &[u8]) -> ffi::evmc_result;
//  pub fn set_option();
//  pub fn set_tracer();
}
*/

// The primary trait that an EVM-C compatible VM must implement.
pub trait EvmcVM {
    fn init() -> Self;
    // TODO: higher level API
    fn execute(
        &self,
        code: &[u8],
        msg: ffi::evmc_message,
        rev: ffi::evmc_revision,
        context: ffi::evmc_context,
        instance: ffi::evmc_instance,
    ) -> ExecutionResult;
}

#[macro_export]
macro_rules! evmc_create_vm {
    ($__vm:ident, $__version:expr) => {
        paste::item! {
            static [<$__vm _NAME>]: &'static str = stringify!($__vm);
            static [<$__vm _VERSION>]: &'static str = $__version;
        }

        paste::item! {
            #[derive(Clone)]
            #[repr(C)]
            pub struct [<$__vm Instance>] {
                inner: evmc_sys::evmc_instance,
                vm: $__vm,
            }
        }

        paste::item! {
            impl [<$__vm Instance>] {
                pub fn new() -> Self {
                    //$__vm must implement EvmcVM
                    [<$__vm Instance>] {
                        inner: evmc_sys::evmc_instance {
                            abi_version: evmc_sys::EVMC_ABI_VERSION as i32,
                            destroy: paste::expr! { Some([<$__vm _destroy>]) },
                            execute: paste::expr! { Some([<$__vm _execute>]) },
                            get_capabilities: None,
                            set_option: None,
                            set_tracer: None,
                            name: {
                                let c_str = paste::expr! { std::ffi::CString::new([<$__vm _NAME>]).expect("Failed to build EVMC name string") };
                                c_str.into_raw() as *const i8
                            },
                            version: {
                                let c_str = paste::expr! { std::ffi::CString::new([<$__vm _VERSION>]).expect("Failed to build EVMC version string") };
                                c_str.into_raw() as *const i8
                            },
                        },
                        vm: $__vm::init(),
                    }
                }

                pub fn get_vm(&self) -> &$__vm {
                    &self.vm
                }

                pub fn get_inner(&self) -> &evmc_sys::evmc_instance {
                    &self.inner
                }

                pub fn into_inner_raw(mut self) -> *mut evmc_sys::evmc_instance {
                    Box::into_raw(Box::new(self)) as *mut evmc_sys::evmc_instance
                }

                // Assumes the pointer is casted from another instance of Self. otherwise UB
                pub unsafe fn coerce_from_raw(raw: *mut ffi::evmc_instance) -> Self {
                    let borrowed = (raw as *mut [<$__vm Instance>]).as_ref();
                    if let Some(instance) = borrowed {
                        let ret = instance.clone();
                        // deallocate the old heap-allocated instance.
                        Box::from_raw(raw);
                        ret
                    } else {
                        panic!();
                    }
                }
            }
        }

        paste::item! {
            extern "C" fn [<$__vm _execute>](
                instance: *mut ffi::evmc_instance,
                context: *mut ffi::evmc_context,
                rev: ffi::evmc_revision,
                msg: *const ffi::evmc_message,
                code: *const u8,
                code_size: usize,
            ) -> ffi::evmc_result {
                let instance = unsafe { [<$__vm Instance>]::coerce_from_raw(instance) };

                //let result = instance.get_vm().execute(...);
                //result.into()
                ffi::evmc_result {
                    create_address: ffi::evmc_address { bytes: [0u8; 20] },
                    gas_left: 0,
                    output_data: 0 as *const u8,
                    output_size: 0,
                    release: None,
                    status_code: ffi::evmc_status_code::EVMC_FAILURE,
                    padding: [0u8; 4],
                }
            }
        }

        paste::item! {
            extern "C" fn [<$__vm _destroy>](instance: *mut ffi::evmc_instance) {
                // The EVMC specification ensures instance cannot be null.
                // Cast to the enclosing struct so that the extra data gets deallocated too.
                let todrop = instance as *mut [<$__vm Instance>];
                drop(unsafe { Box::from_raw(todrop) })
            }
        }

        paste::item! {
            #[no_mangle]
            pub extern "C" fn [<evmc_create_ $__vm>]() -> *const evmc_sys::evmc_instance {
                paste::expr! { [<$__vm Instance>]::new().into_inner_raw() }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use evmc_sys as ffi;

    #[test]
    fn new_result() {
        let r = ExecutionResult::new(
            ffi::evmc_status_code::EVMC_FAILURE,
            420,
            None,
            Some(ffi::evmc_address { bytes: [0u8; 20] }),
        );

        assert!(r.get_status_code() == ffi::evmc_status_code::EVMC_FAILURE);
        assert!(r.get_gas_left() == 420);
        assert!(r.get_output().is_none());

        // Ensure that an address is not returned if it is not valid, per status code.
        assert!(r.get_create_address().is_none());
    }

    #[test]
    fn from_ffi() {
        let f = ffi::evmc_result {
            status_code: ffi::evmc_status_code::EVMC_SUCCESS,
            gas_left: 1337,
            output_data: Box::into_raw(Box::new([0xde, 0xad, 0xbe, 0xef])) as *const u8,
            output_size: 4,
            release: None,
            create_address: ffi::evmc_address { bytes: [0u8; 20] },
            padding: [0u8; 4],
        };

        let r: ExecutionResult = f.into();

        assert!(r.get_status_code() == ffi::evmc_status_code::EVMC_SUCCESS);
        assert!(r.get_gas_left() == 1337);
        assert!(r.get_output().is_some());
        assert!(r.get_output().unwrap().len() == 4);
        assert!(r.get_create_address().is_some());
    }

    #[test]
    fn into_heap_ffi() {
        let r = ExecutionResult::new(
            ffi::evmc_status_code::EVMC_FAILURE,
            420,
            Some(vec![0xc0, 0xff, 0xee, 0x71, 0x75]),
            Some(ffi::evmc_address { bytes: [0u8; 20] }),
        );

        let f: *const ffi::evmc_result = r.into();
        assert!(!f.is_null());
        unsafe {
            assert!((*f).status_code == ffi::evmc_status_code::EVMC_FAILURE);
            assert!((*f).gas_left == 420);
            assert!(!(*f).output_data.is_null());
            assert!((*f).output_size == 5);
            assert!(
                std::slice::from_raw_parts((*f).output_data, 5) as &[u8]
                    == &[0xc0, 0xff, 0xee, 0x71, 0x75]
            );
            assert!((*f).create_address.bytes == [0u8; 20]);
            if (*f).release.is_some() {
                (*f).release.unwrap()(f);
            }
        }
    }

    #[test]
    fn into_heap_ffi_empty_data() {
        let r = ExecutionResult::new(
            ffi::evmc_status_code::EVMC_FAILURE,
            420,
            None,
            Some(ffi::evmc_address { bytes: [0u8; 20] }),
        );

        let f: *const ffi::evmc_result = r.into();
        assert!(!f.is_null());
        unsafe {
            assert!((*f).status_code == ffi::evmc_status_code::EVMC_FAILURE);
            assert!((*f).gas_left == 420);
            assert!((*f).output_data.is_null());
            assert!((*f).output_size == 0);
            assert!((*f).create_address.bytes == [0u8; 20]);
            if (*f).release.is_some() {
                (*f).release.unwrap()(f);
            }
        }
    }

    #[test]
    fn into_stack_ffi() {
        let r = ExecutionResult::new(
            ffi::evmc_status_code::EVMC_FAILURE,
            420,
            Some(vec![0xc0, 0xff, 0xee, 0x71, 0x75]),
            Some(ffi::evmc_address { bytes: [0u8; 20] }),
        );

        let f: ffi::evmc_result = r.into();
        unsafe {
            assert!(f.status_code == ffi::evmc_status_code::EVMC_FAILURE);
            assert!(f.gas_left == 420);
            assert!(!f.output_data.is_null());
            assert!(f.output_size == 5);
            assert!(
                std::slice::from_raw_parts(f.output_data, 5) as &[u8]
                    == &[0xc0, 0xff, 0xee, 0x71, 0x75]
            );
            assert!(f.create_address.bytes == [0u8; 20]);
            if f.release.is_some() {
                f.release.unwrap()(&f);
            }
        }
    }

    #[test]
    fn into_stack_ffi_empty_data() {
        let r = ExecutionResult::new(
            ffi::evmc_status_code::EVMC_FAILURE,
            420,
            None,
            Some(ffi::evmc_address { bytes: [0u8; 20] }),
        );

        let f: ffi::evmc_result = r.into();
        unsafe {
            assert!(f.status_code == ffi::evmc_status_code::EVMC_FAILURE);
            assert!(f.gas_left == 420);
            assert!(f.output_data.is_null());
            assert!(f.output_size == 0);
            assert!(f.create_address.bytes == [0u8; 20]);
            if f.release.is_some() {
                f.release.unwrap()(&f);
            }
        }
    }

    unsafe extern "C" fn get_dummy_tx_context(
        _context: *mut ffi::evmc_context,
    ) -> ffi::evmc_tx_context {
        ffi::evmc_tx_context {
            tx_gas_price: ffi::evmc_uint256be { bytes: [0u8; 32] },
            tx_origin: ffi::evmc_address { bytes: [0u8; 20] },
            block_coinbase: ffi::evmc_address { bytes: [0u8; 20] },
            block_number: 42,
            block_timestamp: 235117,
            block_gas_limit: 105023,
            block_difficulty: ffi::evmc_uint256be { bytes: [0xaa; 32] },
        }
    }

    unsafe extern "C" fn get_dummy_code_size(
        _context: *mut ffi::evmc_context,
        _addr: *const ffi::evmc_address,
    ) -> usize {
        105023 as usize
    }

    // Update these when needed for tests
    fn get_dummy_context() -> ffi::evmc_context {
        ffi::evmc_context {
            host: Box::into_raw(Box::new(ffi::evmc_host_interface {
                account_exists: None,
                get_storage: None,
                set_storage: None,
                get_balance: None,
                get_code_size: Some(get_dummy_code_size),
                get_code_hash: None,
                copy_code: None,
                selfdestruct: None,
                call: None,
                get_tx_context: Some(get_dummy_tx_context),
                get_block_hash: None,
                emit_log: None,
            })),
        }
    }

    fn get_dummy_message() -> ffi::evmc_message {
        ffi::evmc_message {
            kind: ffi::evmc_call_kind::EVMC_CALL,
            flags: 0,
            depth: 123,
            gas: 105023,
            destination: ffi::evmc_address { bytes: [0u8; 20] },
            sender: ffi::evmc_address { bytes: [0u8; 20] },
            input_data: std::ptr::null() as *const u8,
            input_size: 0,
            value: ffi::evmc_uint256be { bytes: [0u8; 32] },
            create2_salt: ffi::evmc_uint256be { bytes: [0u8; 32] },
        }
    }

    #[test]
    fn execution_context() {
        let msg = get_dummy_message();
        let mut context_raw = get_dummy_context();
        let mut exe_context = ExecutionContext::new(&msg, &mut context_raw);

        let a = exe_context.get_tx_context();
        let b = unsafe { get_dummy_tx_context(&mut get_dummy_context() as *mut ffi::evmc_context) };

        assert_eq!(a.block_gas_limit, b.block_gas_limit);
        assert_eq!(a.block_timestamp, b.block_timestamp);
        assert_eq!(a.block_number, b.block_number);

        let c = exe_context.get_message();
        let d = get_dummy_message();

        assert_eq!(c.kind, d.kind);
        assert_eq!(c.flags, d.flags);
        assert_eq!(c.depth, d.depth);
        assert_eq!(c.gas, d.gas);
        assert_eq!(c.input_data, d.input_data);
        assert_eq!(c.input_size, d.input_size);
    }

    #[test]
    fn get_code_size() {
        let msg = get_dummy_message();

        // This address is useless. Just a dummy parameter for the interface function.
        let test_addr = ffi::evmc_address { bytes: [0u8; 20] };
        let mut context_raw = get_dummy_context();
        let mut exe_context = ExecutionContext::new(&msg, &mut context_raw);

        let a: usize = 105023;
        let b = exe_context.get_code_size(&test_addr);

        assert_eq!(a, b);
    }

    #[derive(Clone)]
    pub struct FooVM {
        pub bar: u32,
        pub baz: u64,
    }

    impl EvmcVM for FooVM {
        fn init() -> Self {
            FooVM { bar: 42, baz: 64 }
        }

        fn execute(
            &self,
            code: &[u8],
            msg: ffi::evmc_message,
            rev: ffi::evmc_revision,
            context: ffi::evmc_context,
            instance: ffi::evmc_instance,
        ) -> ExecutionResult {
            unimplemented!();
        }
    }
    evmc_create_vm!(FooVM, "0.5");
    #[test]
    fn create_macro() {
        assert!(FooVM_NAME == "FooVM");
        assert!(FooVM_VERSION == "0.5");

        let instance = evmc_create_FooVM();
        unsafe {
            assert!((*instance).abi_version == ffi::EVMC_ABI_VERSION as i32);
            assert!((*instance).destroy == Some(FooVM_destroy));
            assert!((*instance).execute == Some(FooVM_execute));
            assert!((*instance).get_capabilities.is_none());
            assert!((*instance).set_option.is_none());
            assert!((*instance).set_tracer.is_none());
            assert!((*instance).name != std::ptr::null());
            assert!((*instance).version != std::ptr::null());

            let name_raw =
                std::ffi::CString::from_raw((*instance).name as *mut std::os::raw::c_char);
            let name = name_raw.to_str().unwrap();

            let version_raw =
                std::ffi::CString::from_raw((*instance).version as *mut std::os::raw::c_char);
            let version = version_raw.to_str().unwrap();

            assert!(name == FooVM_NAME);
            assert!(version == FooVM_VERSION);
        }
        // assuming this pointer is coerced from a larger instance struct with the private data, it
        // should be ok to coerce it back to said struct. do not try this at home kids. seriously though
        let coerced = unsafe { Box::from_raw(instance as *mut FooVMInstance) };

        let vmdata: &FooVM = coerced.get_vm();

        assert!(vmdata.bar == 42);
        assert!(vmdata.baz == 64);

        let uncoerced = unsafe { Box::into_raw(coerced) as *mut ffi::evmc_instance };
    }
}
