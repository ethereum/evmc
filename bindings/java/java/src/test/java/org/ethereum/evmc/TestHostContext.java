// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import org.ethereum.evmc.types.Address;
import org.ethereum.evmc.types.Bytes32;
import org.ethereum.evmc.types.TxContext;
import org.ethereum.evmc.types.Uint256;

class TestHostContext implements HostContext {

  private static final Uint256 GAS_PRICE = new Uint256(new byte[32]);

  static {
    Arrays.fill(GAS_PRICE.get(), (byte) 8);
  }

  private static final Uint256 CHAIN_ID;

  static {
    CHAIN_ID = new Uint256(new byte[32]);
    Arrays.fill(CHAIN_ID.get(), (byte) 10);
  }

  private static final Uint256 BLOCK_DIFFICULTY;

  static {
    BLOCK_DIFFICULTY = new Uint256(new byte[32]);
    BLOCK_DIFFICULTY.get()[0] = (byte) 255;
  }

  private static final Address TX_ORIGIN =
      new Address(
          new byte[] {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20});

  private static final Address BLOCK_COINBASE =
      new Address(
          new byte[] {2, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 19});

  final List<Address> addresses = new ArrayList<>();
  final Map<Address, Map<Bytes32, Bytes32>> storage = new HashMap<>();
  final Map<Address, byte[]> codes = new HashMap<>();
  final Map<Address, Uint256> balances = new HashMap<>();
  final TxContext txContext =
      new TxContext(GAS_PRICE, TX_ORIGIN, BLOCK_COINBASE, 3L, 42L, 42L, BLOCK_DIFFICULTY, CHAIN_ID);
  final Map<Long, Bytes32> blockHashes = new HashMap<>();
  final Map<Address, Bytes32> codeHashes = new HashMap<>();

  @Override
  public boolean accountExists(Address address) {
    return addresses.contains(address);
  }

  @Override
  public int accessAccount(Address address) {
    return 0;
  }

  @Override
  public int accessStorage(Address address, Bytes32 key) {
    return 0;
  }

  @Override
  public Bytes32 getStorage(Address address, Bytes32 key) {
    return storage.getOrDefault(address, Collections.emptyMap()).getOrDefault(key, Bytes32.EMPTY);
  }

  @Override
  public StorageStatus setStorage(Address address, Bytes32 key, Bytes32 value) {
    boolean dirty = false;
    boolean added = false;
    boolean deleted = false;
    if (storage.get(address) == null) {
      storage.put(address, new HashMap<>());
      added = true;
    }
    Bytes32 oldValue = storage.get(address).get(key);
    if (oldValue == null && value != null) {
      added = true;
    }
    if (oldValue != null && value == null) {
      deleted = true;
    }
    if (!Objects.equals(oldValue, value)) {
      dirty = true;
      storage.get(address).put(key, value);
    }
    if (added) {
      return StorageStatus.EVMC_STORAGE_ADDED;
    }
    if (deleted) {
      return StorageStatus.EVMC_STORAGE_DELETED;
    }
    if (dirty) {
      return StorageStatus.EVMC_STORAGE_MODIFIED;
    }
    return StorageStatus.EVMC_STORAGE_UNCHANGED;
    // TODO StorageStatus.EVMC_STORAGE_MODIFIED_AGAIN
  }

  @Override
  public Uint256 getBalance(Address address) {
    return balances.get(address);
  }

  @Override
  public int getCodeSize(Address address) {
    byte[] code = codes.get(address);
    return code == null ? 0 : code.length;
  }

  @Override
  public Bytes32 getCodeHash(Address address) {
    return codeHashes.get(address);
  }

  @Override
  public ByteBuffer getCode(Address address) {
    byte[] code = codes.get(address);
    return ByteBuffer.allocateDirect(code.length).put(code);
  }

  @Override
  public void selfdestruct(Address address, Address beneficiary) {}

  @Override
  public ByteBuffer call(ByteBuffer msg) {
    return ByteBuffer.allocateDirect(64).put(new byte[64]);
  }

  @Override
  public TxContext getTxContext() {
    return txContext;
  }

  @Override
  public Bytes32 getBlockHash(long number) {
    return blockHashes.get(number);
  }

  @Override
  public void emitLog(Address address, byte[] data, Bytes32[] topics) {}
}
