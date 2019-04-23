#[macro_use]
extern crate evmc_vm;

use evmc_vm::{EvmcVM, ExecutionResult, InterfaceManager};

// NOTE: this is lowercase because of the requirements of EVMC. will fix this later.
#[derive(Clone)]
struct examplerustvm;

impl EvmcVM for examplerustvm {
    fn init() -> Self {
        examplerustvm
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

evmc_create_vm!(examplerustvm, "0.1.0");
