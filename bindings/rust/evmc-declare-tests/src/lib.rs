/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use evmc_declare::evmc_declare_vm;
use evmc_vm::EvmcVm;
use evmc_vm::ExecutionContext;
use evmc_vm::ExecutionMessage;
use evmc_vm::ExecutionResult;

#[evmc_declare_vm("Foo VM", "ewasm, evm", "1.42-alpha.gamma.starship")]
pub struct FooVM {}

impl EvmcVm for FooVM {
    fn init() -> Self {
        FooVM {}
    }

    fn execute(
        &self,
        _revision: evmc_sys::evmc_revision,
        _code: &[u8],
        _message: &ExecutionMessage,
        _context: Option<&mut ExecutionContext>,
    ) -> ExecutionResult {
        ExecutionResult::success(1337, None)
    }
}
