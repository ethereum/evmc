// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2020 The EVMC Authors.
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
	return StorageUnchanged
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

func (host *testHostContext) Selfdestruct(addr Address, beneficiary Address) {
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
	destination Address, sender Address, value Hash, input []byte, gas int64, depth int,
	static bool, salt Hash) (output []byte, gasLeft int64, createAddr Address, err error) {
	output = []byte("output from testHostContext.Call()")
	return output, gas, Address{}, nil
}

func TestGetTxContext(t *testing.T) {
	vm, _ := Load(modulePath)
	defer vm.Destroy()

	host := &testHostContext{}
	code := []byte("\x43\x60\x00\x52\x59\x60\x00\xf3")

	addr := Address{}
	h := Hash{}
	output, gasLeft, err := vm.Execute(host, Byzantium, Call, false, 1, 100, addr, addr, nil, h, code, h)

	if len(output) != 20 {
		t.Errorf("unexpected output size: %d", len(output))
	}
	if bytes.Compare(output[0:3], []byte("42\x00")) != 0 {
		t.Errorf("execution unexpected output: %s", output)
	}
	if gasLeft != 50 {
		t.Errorf("execution gas left is incorrect: %d", gasLeft)
	}
	if err != nil {
		t.Error("execution returned unexpected error")
	}
}

func TestCall(t *testing.T) {
	vm, _ := Load(modulePath)
	defer vm.Destroy()

	host := &testHostContext{}
	code := []byte("\x60\x00\x80\x80\x80\x80\x80\x80\xf1")

	addr := Address{}
	h := Hash{}
	output, gasLeft, err := vm.Execute(host, Byzantium, Call, false, 1, 100, addr, addr, nil, h, code, h)

	if bytes.Compare(output, []byte("output from testHostContext.Call()")) != 0 {
		t.Errorf("execution unexpected output: %s", output)
	}
	if gasLeft != 99 {
		t.Errorf("execution gas left is incorrect: %d", gasLeft)
	}
	if err != nil {
		t.Error("execution returned unexpected error")
	}
}
