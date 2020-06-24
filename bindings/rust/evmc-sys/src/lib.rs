/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

// TODO: add `.derive_default(true)` to bindgen instead?

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

#[cfg(test)]
mod tests {
    use std::mem::size_of;

    use super::*;

    #[test]
    fn container_new() {
        // TODO: add other checks from test/unittests/test_helpers.cpp
        assert_eq!(size_of::<evmc_bytes32>(), 32);
        assert_eq!(size_of::<evmc_address>(), 20);
        assert!(size_of::<evmc_result>() <= 64);
        assert!(size_of::<evmc_vm>() <= 64);
    }
}
