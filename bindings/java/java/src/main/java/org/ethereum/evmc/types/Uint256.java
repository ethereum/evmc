// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;

public class Uint256 extends FixedLengthType {
  @Override
  protected int expectedLength() {
    return 32;
  }

  public Uint256(byte[] data) {
    super(data);
  }

  public Uint256(ByteBuffer data) {
    super(data);
  }
}
