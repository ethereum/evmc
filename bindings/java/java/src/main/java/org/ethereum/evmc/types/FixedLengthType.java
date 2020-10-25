// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public abstract class FixedLengthType {
  protected abstract int expectedLength();

  private final byte[] data;

  public byte[] get() {
    return this.data;
  }

  public ByteBuffer getByteBuffer() {
    // This is big endian by default, which is what we want.
    return ByteBuffer.allocateDirect(this.data.length).put(this.data);
  }

  public FixedLengthType(byte[] data) {
    if (data == null) {
      throw new IllegalArgumentException("data cannot be null");
    }
    if (data.length != expectedLength()) {
      throw new IllegalArgumentException(
          "data length expected to be " + expectedLength() + ", was " + data.length);
    }
    this.data = data;
  }

  public FixedLengthType(ByteBuffer data) {
    if (data == null) {
      throw new IllegalArgumentException("data cannot be null");
    }
    int length = data.remaining();
    if (length != expectedLength()) {
      throw new IllegalArgumentException(
          "data length expected to be " + expectedLength() + ", was " + length);
    }
    this.data = new byte[length];
    data.order(ByteOrder.BIG_ENDIAN).get(this.data, 0, length);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    FixedLengthType that = (FixedLengthType) o;
    return Arrays.equals(data, that.data);
  }

  @Override
  public int hashCode() {
    return Arrays.hashCode(data);
  }
}
