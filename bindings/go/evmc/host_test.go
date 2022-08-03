// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

package evmc

import (
	"bytes"
	"testing"
)

type testHostContext struct{}

func (host *testHostContext) AccountExists(addr Address) bool {
	return false
}

func (host *testHostContext) GetStorage(addr Address, key Hash) Hash {
	return Hash{}
}

func (host *testHostContext) SetStorage(addr Address, key Hash, value Hash) (status StorageStatus) {
	return StorageAssigned
}

func (host *testHostContext) GetBalance(addr Address) Hash {
	return Hash{}
}

func (host *testHostContext) GetCodeSize(addr Address) int {
	return 0
}

func (host *testHostContext) GetCodeHash(addr Address) Hash {
	return Hash{}
}

func (host *testHostContext) GetCode(addr Address) []byte {
	return nil
}

func (host *testHostContext) Selfdestruct(addr Address, beneficiary Address) bool {
	return false
}

func (host *testHostContext) GetTxContext() TxContext {
	txContext := TxContext{}
	txContext.Number = 42
	return txContext
}

func (host *testHostContext) GetBlockHash(number int64) Hash {
	return Hash{}
}

func (host *testHostContext) EmitLog(addr Address, topics []Hash, data []byte) {
}

func (host *testHostContext) Call(kind CallKind,
	recipient Address, sender Address, value Hash, input []byte, gas int64, depth int,
	static bool, salt Hash, codeAddress Address) (output []byte, gasLeft int64, gasRefund int64,
	createAddr Address, err error) {
	output = []byte("output from testHostContext.Call()")
	return output, gas, 0, Address{}, nil
}

func (host *testHostContext) AccessAccount(addr Address) AccessStatus {
	return ColdAccess
}

func (host *testHostContext) AccessStorage(addr Address, key Hash) AccessStatus {
	return ColdAccess
}

func TestGetBlockNumberFromTxContext(t *testing.T) {
	// Yul: mstore(0, number()) return(0, msize())
	code := []byte("\x43\x60\x00\x52\x59\x60\x00\xf3")

	vm, _ := Load(modulePath)
	defer vm.Destroy()

	host := &testHostContext{}
	addr := Address{}
	h := Hash{}
	output, gasLeft, err := vm.Execute(host, Byzantium, Call, false, 1, 100, addr, addr, nil, h, code)

	if len(output) != 32 {
		t.Errorf("unexpected output size: %d", len(output))
	}

	// Should return value 42 (0x2a) as defined in GetTxContext().
	expectedOutput := []byte("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2a")
	if bytes.Compare(output, expectedOutput) != 0 {
		t.Errorf("execution unexpected output: %x", output)
	}
	if gasLeft != 94 {
		t.Errorf("execution gas left is incorrect: %d", gasLeft)
	}
	if err != nil {
		t.Errorf("execution returned unexpected error: %v", err)
	}
}

func TestCall(t *testing.T) {
	// pseudo-Yul: call(0, 0, 0, 0, 0, 0, 34) return(0, msize())
	code := []byte("\x60\x22\x60\x00\x80\x80\x80\x80\x80\xf1\x59\x60\x00\xf3")

	vm, _ := Load(modulePath)
	defer vm.Destroy()

	host := &testHostContext{}
	addr := Address{}
	h := Hash{}
	output, gasLeft, err := vm.Execute(host, Byzantium, Call, false, 1, 100, addr, addr, nil, h, code)

	if len(output) != 34 {
		t.Errorf("execution unexpected output length: %d", len(output))
	}
	if bytes.Compare(output, []byte("output from testHostContext.Call()")) != 0 {
		t.Errorf("execution unexpected output: %s", output)
	}
	if gasLeft != 89 {
		t.Errorf("execution gas left is incorrect: %d", gasLeft)
	}
	if err != nil {
		t.Errorf("execution returned unexpected error: %v", err)
	}
}
