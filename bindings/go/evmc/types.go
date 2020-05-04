// EVMC: Ethereum Client-VM Connector API.
// Copyright 2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package evmc

// Lengths of hashes and addresses in bytes.
const (
	// hashLength is the expected length of the hash (in bytes)
	hashLength = 32
	// addressLength is the expected length of the address (in bytes)
	addressLength = 20
)

// Hash represents the 32 byte Keccak256 hash of arbitrary data.
type Hash [hashLength]byte

// Address represents the 20 byte address of an Ethereum account.
type Address [addressLength]byte
