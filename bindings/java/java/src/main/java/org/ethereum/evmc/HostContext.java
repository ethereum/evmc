// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

import java.nio.ByteBuffer;

/**
 * This interface represents the callback functions must be implemented in order to interface with
 * the EVM.
 */
public interface HostContext {
  /**
   * Check account existence function.
   *
   * <p>This function is used by the VM to check if there exists an account at given address.
   *
   * @param address The address of the account the query is about.
   * @return true if exists, false otherwise.
   */
  boolean accountExists(byte[] address);

  /**
   * Access account function.
   *
   * <p>This function is used by the VM to add the given address to accessed_addresses substate (see
   * EIP-2929).
   *
   * @param address The address of the account.
   * @return 0 if cold access, 1 if warm access.
   * @todo Change return type to enum.
   */
  int accessAccount(byte[] address);

  /**
   * Access storage function.
   *
   * <p>This function is used by the VM to add the given account storage entry to
   * accessed_storage_keys substate (see EIP-2929).
   *
   * @param address The address of the account.
   * @param key The index of the account's storage entry.
   * @return 0 if cold access, 1 if warm access.
   * @todo Change return type to enum.
   */
  int accessStorage(byte[] address, byte[] key);

  /**
   * Get storage function.
   *
   * <p>This function is used by a VM to query the given account storage entry.
   *
   * @param address The address of the account.
   * @param key The index of the account's storage entry.
   * @return The storage value at the given storage key or null bytes if the account does not exist.
   */
  ByteBuffer getStorage(byte[] address, byte[] key);

  /**
   * Set storage function.
   *
   * <p>This function is used by a VM to update the given account storage entry. The VM MUST make
   * sure that the account exists. This requirement is only a formality because VM implementations
   * only modify storage of the account of the current execution context (i.e. referenced by
   * evmc_message::recipient).
   *
   * @param address The address of the account.
   * @param key The index of the storage entry.
   * @param value The value to be stored.
   * @return The effect on the storage item.
   */
  int setStorage(byte[] address, byte[] key, byte[] value);

  /**
   * Get balance function.
   *
   * <p>This function is used by a VM to query the balance of the given account.
   *
   * @param address The address of the account.
   * @return The balance of the given account or 0 if the account does not exist.
   */
  ByteBuffer getBalance(byte[] address);

  /**
   * Get code size function.
   *
   * <p>This function is used by a VM to get the size of the code stored in the account at the given
   * address.
   *
   * @param address The address of the account.
   * @return The size of the code in the account or 0 if the account does not exist.
   */
  int getCodeSize(byte[] address);

  /**
   * Get code hash function.
   *
   * <p>This function is used by a VM to get the keccak256 hash of the code stored in the account at
   * the given address. For existing accounts not having a code, this function returns keccak256
   * hash of empty data.
   *
   * @param address The address of the account.
   * @return The hash of the code in the account or null bytes if the account does not exist.
   */
  ByteBuffer getCodeHash(byte[] address);

  /**
   * Copy code function.
   *
   * <p>This function is used by an EVM to request a copy of the code of the given account to the
   * memory buffer provided by the EVM. The Client MUST copy the requested code, starting with the
   * given offset, to the provided memory buffer up to the size of the buffer or the size of the
   * code, whichever is smaller.
   *
   * @param address The address of the account.
   * @return A copy of the requested code.
   */
  ByteBuffer getCode(byte[] address);

  /**
   * Selfdestruct function.
   *
   * <p>This function is used by an EVM to SELFDESTRUCT given contract. The execution of the
   * contract will not be stopped, that is up to the EVM.
   *
   * @param address The address of the contract to be selfdestructed.
   * @param beneficiary The address where the remaining ETH is going to be transferred.
   * @return The information if the given address has not been registered as selfdestructed yet.
   *     True if registered for the first time, false otherwise.
   */
  boolean selfdestruct(byte[] address, byte[] beneficiary);

  /**
   * This function supports EVM calls.
   *
   * @param msg The call parameters.
   * @return The result of the call.
   */
  ByteBuffer call(ByteBuffer msg);

  /**
   * Get transaction context function.
   *
   * <p>This function is used by an EVM to retrieve the transaction and block context.
   *
   * @return The transaction context.
   */
  ByteBuffer getTxContext();

  /**
   * Get block hash function.
   *
   * <p>This function is used by a VM to query the hash of the header of the given block. If the
   * information about the requested block is not available, then this is signalled by returning
   * null bytes.
   *
   * @param number The block number.
   * @return The block hash or null bytes if the information about the block is not available.
   */
  ByteBuffer getBlockHash(long number);

  /**
   * Log function.
   *
   * <p>This function is used by an EVM to inform about a LOG that happened during an EVM bytecode
   * execution.
   *
   * @param address The address of the contract that generated the log.
   * @param data The unindexed data attached to the log.
   * @param dataSize The length of the data.
   * @param topics The the array of topics attached to the log.
   * @param topicCount The number of the topics. Valid values are between 0 and 4 inclusively.
   */
  void emitLog(byte[] address, byte[] data, int dataSize, byte[][] topics, int topicCount);
}
