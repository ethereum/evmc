// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Result {
  private final boolean is32bit() {
    return System.getProperty("sun.arch.data.model") == "32";
  }

  protected int expectedLength() {
    return 50; // On 64-bit systems.
  }

  // TODO add getter/setters?

  // TODO: add proper enum
  public int status_code;
  public long gas_left;
  public ByteBuffer output;
  public Address create_address;

  public Result() {}

  public ByteBuffer getByteBuffer() {
    // TODO: make this work between 32/64-bit
    return ByteBuffer.allocateDirect(expectedLength() + this.output.remaining())
        .order(ByteOrder.nativeOrder())
        .putInt(this.status_code)
        .putLong(this.gas_left)
        .putInt(expectedLength())
        .putInt(this.output.remaining())
        .putLong(0)
        .put(this.create_address.getByteBuffer())
        .putInt(0)
        .put(this.output);
  }
}
