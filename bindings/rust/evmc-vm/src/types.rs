use evmc_sys as ffi;

/// EVMC address
pub type Address = ffi::evmc_address;

/// EVMC 32 bytes value (used for hashes)
pub type Bytes32 = ffi::evmc_bytes32;

/// EVMC big-endian 256-bit integer
pub type Uint256 = ffi::evmc_uint256be;

#[cfg(test)]
mod tests {
    use super::*;

    // These tests check for Default, PartialEq and Clone traits.
    #[test]
    fn address_smoke_test() {
        let a = ffi::evmc_address::default();
        let b = Address::default();
        assert_eq!(a.clone(), b.clone());
    }

    #[test]
    fn bytes32_smoke_test() {
        let a = ffi::evmc_bytes32::default();
        let b = Bytes32::default();
        assert_eq!(a.clone(), b.clone());
    }

    #[test]
    fn uint26be_smoke_test() {
        let a = ffi::evmc_uint256be::default();
        let b = Uint256::default();
        assert_eq!(a.clone(), b.clone());
    }
}
