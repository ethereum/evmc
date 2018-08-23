// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

package evmc

import (
	"os"
	"testing"
)

func TestLoad(t *testing.T) {
	i, err := Load(os.Getenv("EVMC_PATH"))
	if err != nil {
		t.Fatal(err.Error())
	}
	defer i.Destroy()
	if i.Name() != "interpreter" {
		t.Fatal("name is not 'interpreter'")
	}
	if i.Version()[0] != '1' {
		t.Fatalf("version is %s", i.Version())
	}
}
