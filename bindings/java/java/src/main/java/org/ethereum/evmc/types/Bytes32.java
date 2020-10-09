// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;

public class Bytes32 extends FixedLengthType {
  @Override
  protected int expectedLength() {
    return 32;
  }

  public Bytes32(byte[] data) {
    super(data);
  }

  public Bytes32(ByteBuffer data) {
    super(data);
  }
}
