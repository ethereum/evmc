// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

import java.nio.ByteBuffer;

/**
 * The Host interface.
 *
 * <p>The set of all callback functions expected by VM instances.
 */
final class Host {
  private static ByteBuffer ensureDirectBuffer(ByteBuffer input) {
    // Reallocate if needed.
    if (!input.isDirect()) {
      return ByteBuffer.allocateDirect(input.remaining()).put(input);
    }
    return input;
  }

  /** Check account existence callback function. */
  static boolean account_exists(HostContext context, byte[] address) {
    return context.accountExists(address);
  }

  /** Get storage callback function. */
  static ByteBuffer get_storage(HostContext context, byte[] address, byte[] key) {
    return ensureDirectBuffer(context.getStorage(address, key));
  }

  /** Set storage callback function. */
  static int set_storage(HostContext context, byte[] address, byte[] key, byte[] value) {
    return context.setStorage(address, key, value);
  }
  /** Get balance callback function. */
  static ByteBuffer get_balance(HostContext context, byte[] address) {
    return ensureDirectBuffer(context.getBalance(address));
  }

  /** Get code size callback function. */
  static int get_code_size(HostContext context, byte[] address) {
    return context.getCodeSize(address);
  }

  /** Get code hash callback function. */
  static ByteBuffer get_code_hash(HostContext context, byte[] address) {
    return ensureDirectBuffer(context.getCodeHash(address));
  }

  /** Copy code callback function. */
  static ByteBuffer copy_code(HostContext context, byte[] address) {
    return ensureDirectBuffer(context.getCode(address));
  }

  /** Selfdestruct callback function. */
  static boolean selfdestruct(HostContext context, byte[] address, byte[] beneficiary) {
    return context.selfdestruct(address, beneficiary);
  }

  /** Call callback function. */
  static ByteBuffer call(HostContext context, ByteBuffer msg) {
    return ensureDirectBuffer(context.call(msg));
  }

  /** Get transaction context callback function. */
  static ByteBuffer get_tx_context(HostContext context) {
    return ensureDirectBuffer(context.getTxContext());
  }

  /** Get block hash callback function. */
  static ByteBuffer get_block_hash(HostContext context, long number) {
    return ensureDirectBuffer(context.getBlockHash(number));
  }

  /** Emit log callback function. */
  static void emit_log(
      HostContext context,
      byte[] address,
      byte[] data,
      int data_size,
      byte[][] topics,
      int topic_count) {
    context.emitLog(address, data, data_size, topics, topic_count);
  }

  /** Access account callback function. */
  static int access_account(HostContext context, byte[] address) {
    return context.accessAccount(address);
  }

  /** Access storage callback function. */
  static int access_storage(HostContext context, byte[] address, byte[] key) {
    return context.accessStorage(address, key);
  }
}
