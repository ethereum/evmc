package evmc

import "math/big"

// Lengths of hashes and addresses in bytes.
const (
	// hashLength is the expected length of the hash (in bytes)
	hashLength = 32
	// addressLength is the expected length of the address (in bytes)
	addressLength = 20
)

// Hash represents the 32 byte Keccak256 hash of arbitrary data.
type Hash [hashLength]byte

// Big converts a hash to a big integer.
func (h Hash) Big() *big.Int { return new(big.Int).SetBytes(h[:]) }

// Address represents the 20 byte address of an Ethereum account.
type Address [addressLength]byte
