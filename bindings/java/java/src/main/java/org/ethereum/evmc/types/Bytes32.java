// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

public class Bytes32 extends FixedLengthType {

  public static final Bytes32 EMPTY = new Bytes32(new byte[32]);

  @Override
  protected int expectedLength() {
    return 32;
  }

  public Bytes32(byte[] data) {
    super(data);
  }
}
