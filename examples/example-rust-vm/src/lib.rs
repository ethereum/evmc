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

    fn execute(
        &self,
        _revision: evmc_sys::evmc_revision,
        _code: &[u8],
        message: &ExecutionMessage,
        _context: &ExecutionContext,
    ) -> ExecutionResult {
        let is_create = message.kind() == evmc_sys::evmc_call_kind::EVMC_CREATE;

        if is_create {
            ExecutionResult::failure()
        } else {
            ExecutionResult::success(66, Some(&[0xc0, 0xff, 0xee]))
        }
    }
}
