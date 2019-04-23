#[macro_use]
extern crate evmc_vm;

use evmc_vm::{EvmcVM, ExecutionResult, InterfaceManager};

// NOTE: this is lowercase because of the requirements of EVMC. will fix this later.
#[derive(Clone)]
struct testvm;

impl EvmcVM for testvm {
    fn init() -> Self {
        testvm
    }

    fn execute(&self, code: &[u8], execution_ctx: &ExecutionContext) -> ExecutionResult {
        let is_create = execution_ctx.get_message().kind == ffi::evmc_call_kind::EVMC_CREATE;
        if is_create {
            evmc_vm::ExecutionResult::new(ffi::evmc_status_code::EVMC_FAILURE, 0, None, None)
        } else {
            evmc_vm::ExecutionResult::new(
                ffi::evmc_status_code::EVMC_SUCCESS,
                69,
                Some(vec![0xc0, 0xff, 0xee]),
                None,
            )
        }
    }
}

evmc_create_vm!(testvm, "0.1.0");

#[no_mangle]
pub extern "C" fn evmc_create_examplerustvm() -> *const ffi::evmc_instance {
    let ret = ffi::evmc_instance {
        abi_version: ffi::EVMC_ABI_VERSION as i32,
        destroy: Some(destroy),
        execute: Some(execute),
        get_capabilities: Some(get_capabilities),
        set_option: None,
        set_tracer: None,
        name: {
            let c_str =
                std::ffi::CString::new("ExampleRustVM").expect("Failed to build EVMC name string");
            c_str.into_raw() as *const i8
        },
        version: {
            let c_str = std::ffi::CString::new("1.0").expect("Failed to build EVMC version string");
            c_str.into_raw() as *const i8
        },
    };
    Box::into_raw(Box::new(ret))
}
