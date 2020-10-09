// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;

public class Address extends FixedLengthType {
  @Override
  protected int expectedLength() {
    return 20;
  }

  public Address(byte[] data) {
    super(data);
  }

  public Address(ByteBuffer data) {
    super(data);
  }
}
