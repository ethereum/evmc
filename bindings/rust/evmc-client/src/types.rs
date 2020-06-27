/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

pub use evmc_vm::{Capabilities, MessageKind, Revision, StatusCode, StorageStatus};

pub const ADDRESS_LENGTH: usize = 20;
pub const BYTES32_LENGTH: usize = 32;
pub type Address = [u8; ADDRESS_LENGTH];
pub type Bytes32 = [u8; BYTES32_LENGTH];
pub type Bytes = [u8];
