// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

//go:generate g++ -shared ../../../examples/example_vm/example_vm.cpp -I../../../include -o example_vm.so

package evmc

import (
	"bytes"
	"testing"
)

var modulePath = "./example_vm.so"

func TestLoad(t *testing.T) {
	i, err := Load(modulePath)
	if err != nil {
		t.Fatal(err.Error())
	}
	defer i.Destroy()
	if i.Name() != "example_vm" {
		t.Fatalf("name is %s", i.Name())
	}
	if i.Version()[0] < '0' || i.Version()[0] > '9' {
		t.Fatalf("version number is weird: %s", i.Version())
	}
}

func TestLoadConfigure(t *testing.T) {
	i, err := LoadAndConfigure(modulePath)
	if err != nil {
		t.Fatal(err.Error())
	}
	defer i.Destroy()
	if i.Name() != "example_vm" {
		t.Fatalf("name is %s", i.Name())
	}
	if i.Version()[0] < '0' || i.Version()[0] > '9' {
		t.Fatalf("version number is weird: %s", i.Version())
	}
}

func TestExecuteEmptyCode(t *testing.T) {
	vm, _ := Load(modulePath)
	defer vm.Destroy()

	addr := Address{}
	h := Hash{}
	output, gasLeft, err := vm.Execute(nil, Byzantium, Call, false, 1, 999, addr, addr, nil, h, nil)

	if bytes.Compare(output, []byte("")) != 0 {
		t.Errorf("execution unexpected output: %x", output)
	}
	if gasLeft != 999 {
		t.Errorf("execution gas left is incorrect: %d", gasLeft)
	}
	if err != nil {
		t.Errorf("execution returned unexpected error: %v", err)
	}
}

func TestRevision(t *testing.T) {
	if MaxRevision != Prague {
		t.Errorf("missing constant for revision %d", MaxRevision)
	}
	if LatestStableRevision != Shanghai {
		t.Errorf("wrong latest stable revision %d", LatestStableRevision)
	}
}

func TestErrorMessage(t *testing.T) {

	check := func(err Error, expectedMsg string) {
		if err.Error() != expectedMsg {
			t.Errorf("wrong error message: '%s', expected: '%s'", err.Error(), expectedMsg)
		}
	}

	check(Failure, "failure")
	check(Revert, "revert")
	check(Error(3), "out of gas")
	check(Error(-1), "internal error")
	check(Error(1000), "<unknown>")
}
