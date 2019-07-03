/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use evmc_declare::evmc_declare_vm;
use evmc_vm::*;

#[evmc_declare_vm("ExampleRustVM", "evm", "6.3.0-dev")]
pub struct ExampleRustVM;

impl EvmcVm for ExampleRustVM {
    fn init() -> Self {
        ExampleRustVM {}
    }

    fn execute(&self, _code: &[u8], context: &ExecutionContext) -> ExecutionResult {
        let is_create = context.get_message().kind() == evmc_sys::evmc_call_kind::EVMC_CREATE;

        if is_create {
            ExecutionResult::failure()
        } else {
            ExecutionResult::success(66, Some(&[0xc0, 0xff, 0xee]))
        }
    }
}
