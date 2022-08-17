// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

import java.nio.ByteBuffer;

class TestHostContext implements HostContext {
  @Override
  public boolean accountExists(byte[] address) {
    return true;
  }

  @Override
  public int accessAccount(byte[] address) {
    return 0;
  }

  @Override
  public int accessStorage(byte[] address, byte[] key) {
    return 0;
  }

  @Override
  public ByteBuffer getStorage(byte[] address, byte[] key) {
    return ByteBuffer.allocateDirect(32).put(new byte[32]);
  }

  @Override
  public int setStorage(byte[] address, byte[] key, byte[] value) {
    return 0;
  }

  @Override
  public ByteBuffer getBalance(byte[] address) {
    return ByteBuffer.allocateDirect(32).put(new byte[32]);
  }

  @Override
  public int getCodeSize(byte[] address) {
    return address.length;
  }

  @Override
  public ByteBuffer getCodeHash(byte[] address) {
    return ByteBuffer.allocateDirect(32).put(new byte[32]);
  }

  @Override
  public ByteBuffer getCode(byte[] address) {
    return ByteBuffer.allocateDirect(64).put(new byte[64]);
  }

  @Override
  public boolean selfdestruct(byte[] address, byte[] beneficiary) {
    return false;
  }

  @Override
  public ByteBuffer call(ByteBuffer msg) {
    return ByteBuffer.allocateDirect(72).put(new byte[72]);
  }

  @Override
  public ByteBuffer getTxContext() {
    return ByteBuffer.allocateDirect(192).put(new byte[192]);
  }

  @Override
  public ByteBuffer getBlockHash(long number) {
    return ByteBuffer.allocateDirect(32).put(new byte[32]);
  }

  @Override
  public void emitLog(byte[] address, byte[] data, int dataSize, byte[][] topics, int topicCount) {}
}
