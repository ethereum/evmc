// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

//go:generate gcc -shared ../../../examples/example_vm.c -I../../../include -o example_vm.so

package evmc

import (
	"testing"
)

func TestLoad(t *testing.T) {
	path := "./example_vm.so"
	i, err := Load(path)
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
