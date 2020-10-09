// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class TxContext {
  protected int expectedLength() {
    return 160;
  }

  // TODO add getter/setters?

  public Uint256 tx_gas_price;
  public Address tx_origin;
  public Address block_coinbase;
  public long block_number;
  public long block_timestamp;
  public long block_gas_limit;
  public Uint256 block_difficulty;
  public Uint256 chain_id;

  public TxContext() {
  }

  public ByteBuffer getByteBuffer() {
    // This is big endian by default, which is what we want.
    return ByteBuffer.allocateDirect(expectedLength())
      .put(this.tx_gas_price.getByteBuffer())
      .put(this.tx_origin.getByteBuffer())
      .put(this.block_coinbase.getByteBuffer())
      .putLong(this.block_number)
      .putLong(this.block_timestamp)
      .putLong(this.block_gas_limit)
      .put(this.block_difficulty.getByteBuffer())
      .put(this.chain_id.getByteBuffer());
  }
}
