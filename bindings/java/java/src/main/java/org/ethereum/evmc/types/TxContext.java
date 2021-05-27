// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc.types;

import java.nio.ByteBuffer;

public class TxContext extends FixedLengthType {

  private static final int EXPECTED_LENGTH = 160;

  public TxContext(
      Uint256 tx_gas_price,
      Address tx_origin,
      Address block_coinbase,
      long block_number,
      long block_timestamp,
      long block_gas_limit,
      Uint256 block_difficulty,
      Uint256 chain_id) {
    super(
        ByteBuffer.allocate(EXPECTED_LENGTH)
            .put(tx_gas_price.get())
            .put(tx_origin.get())
            .put(block_coinbase.get())
            .putLong(block_number)
            .putLong(block_timestamp)
            .putLong(block_gas_limit)
            .put(block_difficulty.get())
            .put(chain_id.get())
            .rewind());
  }

  protected int expectedLength() {
    return EXPECTED_LENGTH;
  }
}
