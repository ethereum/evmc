// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Message {
  private final boolean is32bit() {
    return System.getProperty("sun.arch.data.model") == "32";
  }

  protected int expectedLength() {
    return 136; // On 64-bit systems.
  }

  // TODO add getter/setters?

  public int kind;
  public int flags;
  public int depth;
  public long gas;
  public Address destination;
  public Address sender;
  public ByteBuffer input;
  public Uint256 value;
  public Bytes32 create2_salt;

  public Message() {
  }

  public ByteBuffer getByteBuffer() {
    // This is big endian by default, which is what we want.
    return ByteBuffer.allocateDirect(expectedLength() + this.input.remaining())
      .putInt(this.kind)
      .putInt(this.flags)
      .putInt(this.depth)
      .putLong(this.gas)
      .put(this.destination.getByteBuffer())
      .put(this.sender.getByteBuffer())
       // TODO: change this offset field based on endianness
      .putLong(136)
      .putInt(this.input.remaining())
      .put(this.value.getByteBuffer())
      .put(this.create2_salt.getByteBuffer())
      .put(this.input);
  }
}
