extern crate evmc_vm;

#[no_mangle]
pub extern "C" fn evmc_create_examplerustvm() -> *const evmc_vm::ffi::evmc_instance {
    std::ptr::null()
}
