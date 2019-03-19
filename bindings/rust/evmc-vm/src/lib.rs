pub extern crate evmc_sys;
pub use evmc_sys as ffi;

// TODO: Add helpers for host interface
// TODO: Add convenient helpers for evmc_execute
// TODO: Add a derive macro here for creating evmc_create

/// EVMC result structure.
pub struct ExecutionResult {
    status_code: ffi::evmc_status_code,
    gas_left: i64,
    output: Option<Vec<u8>>,
    create_address: ffi::evmc_address,
}

impl ExecutionResult {
    pub fn new(
        _status_code: ffi::evmc_status_code,
        _gas_left: i64,
        _output: Option<Vec<u8>>,
        _create_address: ffi::evmc_address,
    ) -> Self {
        ExecutionResult {
            status_code: _status_code,
            gas_left: _gas_left,
            output: _output,
            create_address: _create_address,
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

/// Returns a pointer to a heap-allocated evmc_result.
impl Into<*const ffi::evmc_result> for ExecutionResult {
    fn into(self) -> *const ffi::evmc_result {
        let (buffer, len) = if let Some(buf) = self.output {
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
        };

        Box::into_raw(Box::new(ffi::evmc_result {
            status_code: self.status_code,
            gas_left: self.gas_left,
            output_data: buffer,
            output_size: len,
            release: Some(release_result),
            create_address: self.create_address,
            padding: [0u8; 4],
        }))
    }
}

/// Callback to pass across FFI, de-allocating the struct.
extern "C" fn release_result(result: *const ffi::evmc_result) {
    unsafe {
        let tmp = Box::from_raw(result as *mut ffi::evmc_result);
        if tmp.output_data.is_null() {
            let buf_layout =
                std::alloc::Layout::from_size_align(tmp.output_size, 1).expect("Bad layout");
            std::alloc::dealloc(tmp.output_data as *mut u8, buf_layout);
        }
        Box::from_raw(result as *mut ffi::evmc_result);
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
            ffi::evmc_address { bytes: [0u8; 20] },
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
    fn into_ffi() {
        let r = ExecutionResult::new(
            ffi::evmc_status_code::EVMC_FAILURE,
            420,
            Some(vec![0xc0, 0xff, 0xee, 0x71, 0x75]),
            ffi::evmc_address { bytes: [0u8; 20] },
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
        }
    }
}
