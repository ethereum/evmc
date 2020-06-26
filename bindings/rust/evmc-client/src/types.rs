/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

pub use evmc_sys as ffi;

pub const ADDRESS_LENGTH: usize = 20;
pub const BYTES32_LENGTH: usize = 32;
pub type Address = [u8; ADDRESS_LENGTH];
pub type Bytes32 = [u8; BYTES32_LENGTH];
pub type Bytes = [u8];

pub type StorageStatus = ffi::evmc_storage_status;
pub type CallKind = ffi::evmc_call_kind;
pub type Revision = ffi::evmc_revision;
pub type StatusCode = ffi::evmc_status_code;
pub type Capabilities = ffi::evmc_capabilities;
