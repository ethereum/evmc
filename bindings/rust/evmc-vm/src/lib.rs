/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

//! Rust bindings for EVMC (Ethereum Client-VM Connector API).
//!
//! Have a look at evmc-declare to declare an EVMC compatible VM.
//! This crate documents how to use certain data types.

mod container;

pub use container::EvmcContainer;
pub use evmc_sys as ffi;

pub trait EvmcVm {
    fn init() -> Self;
    fn execute(&self, code: &[u8], context: &ExecutionContext) -> ExecutionResult;
}

/// EVMC result structure.
pub struct ExecutionResult {
    status_code: ffi::evmc_status_code,
    gas_left: i64,
    output: Option<Vec<u8>>,
    create_address: Option<ffi::evmc_address>,
}

/// EVMC execution message structure.
pub struct ExecutionMessage {
    kind: ffi::evmc_call_kind,
    flags: u32,
    depth: i32,
    gas: i64,
    destination: ffi::evmc_address,
    sender: ffi::evmc_address,
    input: Option<Vec<u8>>,
    value: ffi::evmc_uint256be,
    create2_salt: ffi::evmc_bytes32,
}

/// EVMC context structure. Exposes the EVMC host functions, message data, and transaction context
/// to the executing VM.
pub struct ExecutionContext<'a> {
    message: ExecutionMessage,
    context: &'a mut ffi::evmc_context,
    tx_context: ffi::evmc_tx_context,
}

impl ExecutionResult {
    pub fn new(
        _status_code: ffi::evmc_status_code,
        _gas_left: i64,
        _output: Option<&[u8]>,
    ) -> Self {
        ExecutionResult {
            status_code: _status_code,
            gas_left: _gas_left,
            output: if _output.is_some() {
                Some(_output.unwrap().to_vec())
            } else {
                None
            },
            create_address: None,
        }
    }

    pub fn failure() -> Self {
        ExecutionResult::new(ffi::evmc_status_code::EVMC_FAILURE, 0, None)
    }

    pub fn success(_gas_left: i64, _output: Option<&[u8]>) -> Self {
        ExecutionResult::new(ffi::evmc_status_code::EVMC_SUCCESS, _gas_left, _output)
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
        self.create_address.as_ref()
    }
}

impl ExecutionMessage {
    pub fn kind(&self) -> ffi::evmc_call_kind {
        self.kind
    }

    pub fn flags(&self) -> u32 {
        self.flags
    }

    pub fn depth(&self) -> i32 {
        self.depth
    }

    pub fn gas(&self) -> i64 {
        self.gas
    }

    pub fn destination(&self) -> &ffi::evmc_address {
        &self.destination
    }

    pub fn sender(&self) -> &ffi::evmc_address {
        &self.sender
    }

    pub fn input(&self) -> Option<&Vec<u8>> {
        self.input.as_ref()
    }

    pub fn value(&self) -> &ffi::evmc_uint256be {
        &self.value
    }

    pub fn create2_salt(&self) -> &ffi::evmc_bytes32 {
        &self.create2_salt
    }
}

impl<'a> ExecutionContext<'a> {
    pub fn new(_message: &'a ffi::evmc_message, _context: &'a mut ffi::evmc_context) -> Self {
        let _tx_context = unsafe {
            assert!((*(_context.host)).get_tx_context.is_some());
            (*(_context.host)).get_tx_context.unwrap()(_context as *mut ffi::evmc_context)
        };

        ExecutionContext {
            message: _message.into(),
            context: _context,
            tx_context: _tx_context,
        }
    }

    pub fn get_message(&self) -> &ExecutionMessage {
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
            // Consider it is always valid.
            create_address: Some(result.create_address),
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

fn allocate_output_data(output: Option<&Vec<u8>>) -> (*const u8, usize) {
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
        let mut result: ffi::evmc_result = self.into();
        result.release = Some(release_heap_result);
        Box::into_raw(Box::new(result))
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
        let (buffer, len) = allocate_output_data(self.output.as_ref());
        ffi::evmc_result {
            status_code: self.status_code,
            gas_left: self.gas_left,
            output_data: buffer,
            output_size: len,
            release: Some(release_stack_result),
            create_address: if self.create_address.is_some() {
                self.create_address.unwrap()
            } else {
                ffi::evmc_address { bytes: [0u8; 20] }
            },
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

impl From<&ffi::evmc_message> for ExecutionMessage {
    fn from(message: &ffi::evmc_message) -> Self {
        ExecutionMessage {
            kind: message.kind,
            flags: message.flags,
            depth: message.depth,
            gas: message.gas,
            destination: message.destination,
            sender: message.sender,
            input: if message.input_data.is_null() {
                assert!(message.input_size == 0);
                None
            } else {
                // TODO: what to do if input_size is 0?

                // Pre-allocate a vector.
                let mut buf: Vec<u8> = Vec::with_capacity(message.input_size);

                unsafe {
                    // Set the len of the vec manually.
                    buf.set_len(message.input_size);
                    // Copy from the C struct's buffer to the vec's buffer.
                    std::ptr::copy(message.input_data, buf.as_mut_ptr(), message.input_size);
                }

                Some(buf)
            },
            value: message.value,
            create2_salt: message.create2_salt,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn new_result() {
        let r = ExecutionResult::new(ffi::evmc_status_code::EVMC_FAILURE, 420, None);

        assert!(r.get_status_code() == ffi::evmc_status_code::EVMC_FAILURE);
        assert!(r.get_gas_left() == 420);
        assert!(r.get_output().is_none());
        assert!(r.get_create_address().is_none());
    }

    // Test-specific helper to dispose of execution results in unit tests
    extern "C" fn test_result_dispose(result: *const ffi::evmc_result) {
        unsafe {
            if !result.is_null() {
                let owned = *result;
                Vec::from_raw_parts(
                    owned.output_data as *mut u8,
                    owned.output_size,
                    owned.output_size,
                );
            }
        }
    }

    #[test]
    fn from_ffi() {
        let f = ffi::evmc_result {
            status_code: ffi::evmc_status_code::EVMC_SUCCESS,
            gas_left: 1337,
            output_data: Box::into_raw(Box::new([0xde, 0xad, 0xbe, 0xef])) as *const u8,
            output_size: 4,
            release: Some(test_result_dispose),
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
            Some(&[0xc0, 0xff, 0xee, 0x71, 0x75]),
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
        let r = ExecutionResult::new(ffi::evmc_status_code::EVMC_FAILURE, 420, None);

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
            Some(&[0xc0, 0xff, 0xee, 0x71, 0x75]),
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
        let r = ExecutionResult::new(ffi::evmc_status_code::EVMC_FAILURE, 420, None);

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

    // Helper to safely dispose of the dummy context, and not bring up false positives in the
    // sanitizers.
    fn dummy_context_dispose(context: ffi::evmc_context) {
        unsafe {
            Box::from_raw(context.host as *mut ffi::evmc_host_interface);
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
        // Make a copy here so we don't let get_dummy_context() go out of scope when called again
        // in get_dummy_tx_context() and cause LLVM
        // sanitizers to complain
        let mut context_raw_copy = context_raw.clone();

        let mut exe_context = ExecutionContext::new(&msg, &mut context_raw);
        let a = exe_context.get_tx_context();
        let b = unsafe { get_dummy_tx_context(&mut context_raw_copy as *mut ffi::evmc_context) };

        assert_eq!(a.block_gas_limit, b.block_gas_limit);
        assert_eq!(a.block_timestamp, b.block_timestamp);
        assert_eq!(a.block_number, b.block_number);

        let c = exe_context.get_message();
        let d = get_dummy_message();

        assert_eq!(c.kind, d.kind);
        assert_eq!(c.flags, d.flags);
        assert_eq!(c.depth, d.depth);
        assert_eq!(c.gas, d.gas);
        if d.input_data.is_null() {
            assert!(c.input().is_none());
        } else {
            assert!(c.input().is_some());
            assert_eq!(c.input().unwrap().len(), d.input_size);
        }

        dummy_context_dispose(context_raw);
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

        dummy_context_dispose(context_raw);
    }
}
