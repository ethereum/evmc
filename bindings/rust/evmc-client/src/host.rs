// Copyright (C) 2020 Second State.
// This file is part of Rust-SSVM.

// Rust-SSVM is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Rust-SSVM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

use crate::types::*;
use evmc_sys as ffi;
use std::mem;

pub trait HostInterface {
    fn account_exists(&mut self, addr: &Address) -> bool;
    fn get_storage(&mut self, addr: &Address, key: &Bytes32) -> Bytes32;
    fn set_storage(&mut self, addr: &Address, key: &Bytes32, value: &Bytes32) -> StorageStatus;
    fn get_balance(&mut self, addr: &Address) -> Bytes32;
    fn get_code_size(&mut self, addr: &Address) -> usize;
    fn get_code_hash(&mut self, addr: &Address) -> Bytes32;
    fn copy_code(
        &mut self,
        addr: &Address,
        offset: &usize,
        buffer_data: &*mut u8,
        buffer_size: &usize,
    ) -> usize;
    fn selfdestruct(&mut self, addr: &Address, beneficiary: &Address);
    fn get_tx_context(&mut self) -> (Bytes32, Address, Address, i64, i64, i64, Bytes32, Bytes32);
    fn get_block_hash(&mut self, number: i64) -> Bytes32;
    fn emit_log(&mut self, addr: &Address, topics: &Vec<Bytes32>, data: &Bytes);
    fn call(
        &mut self,
        kind: CallKind,
        destination: &Address,
        sender: &Address,
        value: &Bytes32,
        input: &Bytes,
        gas: i64,
        depth: i32,
        is_static: bool,
    ) -> (Vec<u8>, i64, Address, StatusCode);
}

struct Callback {
    host_interface: ::std::option::Option<Box<dyn HostInterface>>,
}

static mut CALLBACK: Callback = Callback { host_interface: None };

pub fn set_host_interface(interface: Option<Box<dyn HostInterface>>) {
    unsafe {
        CALLBACK.host_interface = interface;
    }
}

pub fn get_evmc_host_context() -> ffi::evmc_host_context {
    ffi::evmc_host_context {_bindgen_opaque_blob:[0u8; 0]}
}

pub fn get_evmc_host_interface() -> ffi::evmc_host_interface {
    ffi::evmc_host_interface {
        account_exists: Some(account_exists),
        get_storage: Some(get_storage),
        set_storage: Some(set_storage),
        get_balance: Some(get_balance),
        get_code_size: Some(get_code_size),
        get_code_hash: Some(get_code_hash),
        copy_code: Some(copy_code),
        selfdestruct: Some(selfdestruct),
        call: Some(call),
        get_tx_context: Some(get_tx_context),
        get_block_hash: Some(get_block_hash),
        emit_log: Some(emit_log),
    }
}

unsafe extern "C" fn account_exists(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
) -> bool {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return host_interface.account_exists(&(*address).bytes);
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn get_storage(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
    key: *const ffi::evmc_bytes32,
) -> ffi::evmc_bytes32 {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return ffi::evmc_bytes32 {
                bytes: host_interface.get_storage(&(*address).bytes, &(*key).bytes),
            };
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn set_storage(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
    key: *const ffi::evmc_bytes32,
    value: *const ffi::evmc_bytes32,
) -> ffi::evmc_storage_status {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return host_interface.set_storage(&(*address).bytes, &(*key).bytes, &(*value).bytes);
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn get_balance(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
) -> ffi::evmc_uint256be {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return ffi::evmc_uint256be {
                bytes: host_interface.get_balance(&(*address).bytes),
            };
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn get_code_size(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
) -> usize {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return host_interface.get_code_size(&(*address).bytes);
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn get_code_hash(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
) -> ffi::evmc_bytes32 {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return ffi::evmc_bytes32 {
                bytes: host_interface.get_code_hash(&(*address).bytes),
            };
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn copy_code(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
    code_offset: usize,
    buffer_data: *mut u8,
    buffer_size: usize,
) -> usize {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return host_interface.copy_code(
                &(*address).bytes,
                &code_offset,
                &buffer_data,
                &buffer_size,
            );
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn selfdestruct(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
    beneficiary: *const ffi::evmc_address,
) {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => host_interface.selfdestruct(&(*address).bytes, &(*beneficiary).bytes),
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn get_tx_context(_context: *mut ffi::evmc_host_context) -> ffi::evmc_tx_context {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            let (gas_price, origin, coinbase, number, timestamp, gas_limit, difficulty, chain_id) =
                host_interface.get_tx_context();
            return ffi::evmc_tx_context {
                tx_gas_price: evmc_sys::evmc_bytes32 { bytes: gas_price },
                tx_origin: evmc_sys::evmc_address { bytes: origin },
                block_coinbase: evmc_sys::evmc_address { bytes: coinbase },
                block_number: number,
                block_timestamp: timestamp,
                block_gas_limit: gas_limit,
                block_difficulty: evmc_sys::evmc_bytes32 { bytes: difficulty },
                chain_id: evmc_sys::evmc_bytes32 { bytes: chain_id },
            };
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn get_block_hash(
    _context: *mut ffi::evmc_host_context,
    number: i64,
) -> ffi::evmc_bytes32 {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            return ffi::evmc_bytes32 {
                bytes: host_interface.get_block_hash(number),
            };
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn emit_log(
    _context: *mut ffi::evmc_host_context,
    address: *const ffi::evmc_address,
    data: *const u8,
    data_size: usize,
    topics: *const ffi::evmc_bytes32,
    topics_count: usize,
) {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            let ts = &std::slice::from_raw_parts(topics, topics_count)
                .iter()
                .map(|topic| topic.bytes)
                .collect::<Vec<_>>();
            host_interface.emit_log(
                &(*address).bytes,
                &ts,
                &std::slice::from_raw_parts(data, data_size),
            );
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}

unsafe extern "C" fn release(result: *const ffi::evmc_result) {
    drop(std::slice::from_raw_parts(
        (*result).output_data,
        (*result).output_size,
    ));
}

pub unsafe extern "C" fn call(
    _context: *mut ffi::evmc_host_context,
    msg: *const ffi::evmc_message,
) -> ffi::evmc_result {
    match &mut CALLBACK.host_interface {
        Some(host_interface) => {
            let msg = *msg;
            let (output, gas_left, create_address, status_code) = host_interface.call(
                msg.kind,
                &msg.destination.bytes,
                &msg.sender.bytes,
                &msg.value.bytes,
                &std::slice::from_raw_parts(msg.input_data, msg.input_size),
                msg.gas,
                msg.depth,
                msg.flags != 0,
            );
            let ptr = output.as_ptr();
            let len = output.len();
            mem::forget(output);
            return ffi::evmc_result {
                status_code: status_code,
                gas_left: gas_left,
                output_data: ptr,
                output_size: len,
                release: Some(release),
                create_address: ffi::evmc_address {
                    bytes: create_address,
                },
                padding: [0u8; 4],
            };
        }
        None => {
            panic!("Host context not implemented");
        }
    }
}
