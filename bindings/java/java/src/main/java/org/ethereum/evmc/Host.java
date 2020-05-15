// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

import static java.util.Objects.requireNonNull;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * The Host interface.
 *
 * <p>The set of all callback functions expected by VM instances.
 */
final class Host {
  /** Check account existence callback function. */
  static int account_exists(int context_index, byte[] address) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.accountExists(address) ? 1 : 0;
  }

  /** Get storage callback function. */
  static ByteBuffer get_storage(int context_index, byte[] address, byte[] key) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.getStorage(address, key);
  }

  /** Set storage callback function. */
  static int set_storage(int context_index, byte[] address, byte[] key, byte[] value) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.setStorage(address, key, value);
  }
  /** Get balance callback function. */
  static ByteBuffer get_balance(int context_index, byte[] address) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.getBalance(address);
  }

  /** Get code size callback function. */
  static int get_code_size(int context_index, byte[] address) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.getCodeSize(address);
  }

  /** Get code hash callback function. */
  static ByteBuffer get_code_hash(int context_index, byte[] address) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.getCodeHash(address);
  }

  /** Copy code callback function. */
  static ByteBuffer copy_code(int context_index, byte[] address, int code_offset) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    byte[] code = context.getCode(address).array();

    if (code != null && code_offset > 0 && code_offset < code.length) {
      int length = code.length - code_offset;
      return ByteBuffer.wrap(code, 0, length);
    }

    return ByteBuffer.wrap(new byte[0]);
  }

  /** Selfdestruct callback function. */
  static void selfdestruct(int context_index, byte[] address, byte[] beneficiary) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    context.selfdestruct(address, beneficiary);
  }

  /** Call callback function. */
  static ByteBuffer call(int context_index, ByteBuffer msg) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.call(msg);
  }

  /** Get transaction context callback function. */
  static ByteBuffer get_tx_context(int context_index) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.getTxContext();
  }

  /** Get block hash callback function. */
  static ByteBuffer get_block_hash_fn(int context_index, long number) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    return context.getBlockHash(number);
  }

  /** Emit log callback function. */
  static void emit_log(
      int context_index,
      byte[] address,
      byte[] data,
      int data_size,
      byte[][] topics,
      int topic_count) {
    HostContext context =
        requireNonNull(
            getContext(context_index),
            "HostContext does not exist for context_index: " + context_index);
    context.emitLog(address, data, data_size, topics, topic_count);
  }

  static HostContext getContext(int index) {
    return contextList.get(index);
  }

  static synchronized int addContext(HostContext context) {
    contextList.add(context);
    return contextList.size() - 1;
  }

  static void removeContext(int index) {
    contextList.remove(index);
  }

  private static List<HostContext> contextList = Collections.synchronizedList(new ArrayList<>());
}
