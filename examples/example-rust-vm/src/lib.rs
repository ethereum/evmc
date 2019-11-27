/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

use evmc_declare::evmc_declare_vm;
use evmc_vm::*;

#[evmc_declare_vm("ExampleRustVM", "evm, precompiles", "6.3.0-dev")]
pub struct ExampleRustVM;

impl EvmcVm for ExampleRustVM {
    fn init() -> Self {
        ExampleRustVM {}
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

        if message.kind() != MessageKind::EVMC_CALL {
            return ExecutionResult::failure();
        }

        if _code.len() == 0 {
            return ExecutionResult::failure();
        }

        let tx_context = _context.get_tx_context().clone();

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
        _context.set_storage(&message.destination(), &storage_key, &storage_value);

        let ret = format!("{}", block_number).into_bytes();
        ExecutionResult::success(message.gas() / 2, Some(&ret))
    }
}
