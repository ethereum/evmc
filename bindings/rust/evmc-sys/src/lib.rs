/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

// TODO: with bindgen's interface improving these may be moved to
// bindgen configuration

impl PartialEq for evmc_address {
    fn eq(&self, other: &Self) -> bool {
        self.bytes == other.bytes
    }
}

impl PartialEq for evmc_bytes32 {
    fn eq(&self, other: &Self) -> bool {
        self.bytes == other.bytes
    }
}

impl Default for evmc_address {
    fn default() -> Self {
        evmc_address { bytes: [0u8; 20] }
    }
}

impl Default for evmc_bytes32 {
    fn default() -> Self {
        evmc_bytes32 { bytes: [0u8; 32] }
    }
}
