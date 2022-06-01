/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

pub use evmc_vm::{Capabilities, MessageKind, Revision, StatusCode, StorageStatus};

pub type Address = [u8; 20];
pub type Bytes32 = [u8; 32];
pub type Bytes = [u8];
