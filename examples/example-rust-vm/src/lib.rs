// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

use core::str::FromStr;
use evmc_declare::evmc_declare_vm;
use evmc_vm::*;

#[evmc_declare_vm("ExampleRustVM", "evm, precompiles", "10.1.0")]
pub struct ExampleRustVM {
    verbosity: i8,
}

impl EvmcVm for ExampleRustVM {
    fn init() -> Self {
        Self { verbosity: 0 }
    }

    fn set_option(&mut self, key: &str, value: &str) -> Result<(), SetOptionError> {
        if key == "verbose" {
            if value.is_empty() {
                return Err(SetOptionError::InvalidValue);
            }

            self.verbosity = i8::from_str(value).map_err(|_| SetOptionError::InvalidValue)?;
            if self.verbosity > 9 {
                return Err(SetOptionError::InvalidValue);
            }

            return Ok(());
        }

        Err(SetOptionError::InvalidKey)
    }

    fn execute<'a>(
        &self,
        _revision: Revision,
        _code: &'a [u8],
        message: &'a ExecutionMessage,
        _context: Option<&'a mut ExecutionContext<'a>>,
    ) -> ExecutionResult {
        if _context.is_none() {
            return ExecutionResult::failure();
        }
        let _context = _context.unwrap();

        if self.verbosity > 0 {
            println!("execution started");
        }

        if message.kind() != MessageKind::EVMC_CALL {
            return ExecutionResult::failure();
        }

        if _code.is_empty() {
            return ExecutionResult::failure();
        }

        let tx_context = *_context.get_tx_context();

        let save_return_block_number: Vec<u8> = vec![
            0x43, 0x60, 0x00, 0x55, 0x43, 0x60, 0x00, 0x52, 0x59, 0x60, 0x00, 0xf3,
        ];

        if save_return_block_number != _code {
            return ExecutionResult::failure();
        }

        assert!(tx_context.block_number <= 255);
        let block_number = tx_context.block_number as u8;

        let storage_key = Bytes32::default();
        let mut storage_value = Bytes32::default();
        storage_value.bytes[31] = block_number;
        _context.set_storage(message.recipient(), &storage_key, &storage_value);

        let ret = format!("{}", block_number).into_bytes();
        ExecutionResult::success(message.gas() / 2, 0, Some(&ret))
    }
}
