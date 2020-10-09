// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public abstract class FixedLengthType {
  protected abstract int expectedLength();

  private byte[] data;

  public byte[] get() {
    return this.data;
  }

  public ByteBuffer getByteBuffer() {
    // This is big endian by default, which is what we want.
    return ByteBuffer.allocateDirect(this.data.length).put(this.data);
  }

  public FixedLengthType(byte[] data) {
    if (data.length != expectedLength())
      throw new IllegalArgumentException();
    this.data = data;
  }

  public FixedLengthType(ByteBuffer data) {
    int length = data.remaining();
    if (length != expectedLength())
      throw new IllegalArgumentException();
    this.data = new byte[length];
    data.order(ByteOrder.BIG_ENDIAN).get(this.data, 0, length);
  }
}
