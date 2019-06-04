/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use evmc_vm::EvmcVm;
use evmc_vm::ExecutionContext;
use evmc_vm::ExecutionResult;
#[macro_use]
use evmc_declare::evmc_declare_vm;

#[evmc_declare_vm("Foo VM", "ewasm, evm", "1.42-alpha.gamma.starship")]
pub struct FooVM {
    a: i32,
}

impl EvmcVm for FooVM {
    fn init() -> Self {
        FooVM { a: 105023 }
    }

    fn execute(&self, code: &[u8], context: &ExecutionContext) -> ExecutionResult {
        ExecutionResult::success(1337, None)
    }
}
