// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

use evmc_declare::evmc_declare_vm;
use evmc_vm::EvmcVm;
use evmc_vm::ExecutionContext;
use evmc_vm::ExecutionMessage;
use evmc_vm::ExecutionResult;
use evmc_vm::SetOptionError;
use std::collections::HashMap;

#[evmc_declare_vm("Foo VM", "ewasm, evm", "1.42-alpha.gamma.starship")]
pub struct FooVM {
    options: HashMap<String, String>,
}

impl EvmcVm for FooVM {
    fn init() -> Self {
        Self {
            options: Default::default(),
        }
    }

    fn set_option(&mut self, key: &str, value: &str) -> Result<(), SetOptionError> {
        self.options.insert(key.to_string(), value.to_string());

        Ok(())
    }

    fn execute(
        &self,
        _revision: evmc_sys::evmc_revision,
        _code: &[u8],
        _message: &ExecutionMessage,
        _context: Option<&mut ExecutionContext>,
    ) -> ExecutionResult {
        ExecutionResult::success(1337, 21, None)
    }
}
