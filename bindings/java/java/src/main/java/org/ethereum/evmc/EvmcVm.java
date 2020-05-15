// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

import static org.ethereum.evmc.Host.addContext;
import static org.ethereum.evmc.Host.removeContext;

import java.nio.ByteBuffer;
import java.util.Objects;

/**
 * The Java interface to the evm instance.
 *
 * <p>Defines the Java methods capable of accessing the evm implementation.
 */
public final class EvmcVm implements AutoCloseable {
  private static EvmcVm evmcVm;
  private static boolean isEvmcLibraryLoaded = false;
  private ByteBuffer nativeVm;
  /**
   * This method loads the specified evm shared library and loads/initializes the jni bindings.
   *
   * @param filename /path/filename of the evm shared object
   */
  public static EvmcVm create(String filename) {
    if (!EvmcVm.isEvmcLibraryLoaded) {
      try {
        // load so containing the jni bindings to evmc
        System.load(System.getProperty("user.dir") + "/../c/build/lib/libevmc-java.so");
        EvmcVm.isEvmcLibraryLoaded = true;
      } catch (UnsatisfiedLinkError e) {
        System.err.println("Native code library failed to load.\n" + e);
        System.exit(1);
      }
    }
    if (Objects.isNull(evmcVm)) {
      evmcVm = new EvmcVm(filename);
    }
    return evmcVm;
  }

  private EvmcVm(String filename) {
    // initialize jni and load EVM shared library
    nativeVm = init(filename);
  }

  /**
   * This method loads the specified evm implementation and initializes jni
   *
   * @param filename path + filename of the evm shared object to load
   * @return
   */
  public native ByteBuffer init(String filename);

  /**
   * EVMC ABI version implemented by the VM instance.
   *
   * <p>Can be used to detect ABI incompatibilities. The EVMC ABI version represented by this file
   * is in ::EVMC_ABI_VERSION.
   */
  public native int abi_version();

  /**
   * The name of the EVMC VM implementation.
   *
   * <p>It MUST be a NULL-terminated not empty string. The content MUST be UTF-8 encoded (this
   * implies ASCII encoding is also allowed).
   */
  native String name(ByteBuffer nativeVm);

  /** Function is a wrapper around native name(). */
  public String name() {
    return name(nativeVm);
  }

  /**
   * The version of the EVMC VM implementation, e.g. "1.2.3b4".
   *
   * <p>It MUST be a NULL-terminated not empty string. The content MUST be UTF-8 encoded (this
   * implies ASCII encoding is also allowed).
   */
  native String version(ByteBuffer nativeVm);

  /** Function is a wrapper around native version(). */
  public String version() {
    return version(nativeVm);
  }

  /**
   * Function to destroy the VM instance.
   *
   * <p>This is a mandatory method and MUST NOT be set to NULL.
   */
  native void destroy(ByteBuffer nativeVm);

  /**
   * Function to execute a code by the VM instance.
   *
   * <p>This is a mandatory method and MUST NOT be set to NULL.
   */
  native void execute(
      ByteBuffer nativeVm,
      int context_index,
      int rev,
      ByteBuffer msg,
      ByteBuffer code,
      int size,
      ByteBuffer result);

  /**
   * Function is a wrapper around native execute.
   *
   * <p>This allows the context to managed in one method
   */
  public synchronized ByteBuffer execute(
      HostContext context, int rev, ByteBuffer msg, ByteBuffer code, int size) {
    int context_index = addContext(context);
    int resultSize = get_result_size();
    ByteBuffer result = ByteBuffer.allocateDirect(resultSize);
    execute(nativeVm, context_index, rev, msg, code, size, result);
    removeContext(context_index);
    return result;
  }

  /**
   * A method returning capabilities supported by the VM instance.
   *
   * <p>The value returned MAY change when different options are set via the set_option() method.
   *
   * <p>A Client SHOULD only rely on the value returned if it has queried it after it has called the
   * set_option().
   *
   * <p>This is a mandatory method and MUST NOT be set to NULL.
   */
  native int get_capabilities(ByteBuffer nativeVm);

  /** Function is a wrapper around native get_capabilities(). */
  public int get_capabilities() {
    return get_capabilities(nativeVm);
  }

  /**
   * Function that modifies VM's options.
   *
   * <p>If the VM does not support this feature the pointer can be NULL.
   */
  native int set_option(ByteBuffer nativeVm, String name, String value);

  /** Function is a wrapper around native set_option(). */
  public int set_option(String name, String value) {
    return set_option(nativeVm, name, value);
  }

  /** get size of result struct */
  native int get_result_size();

  /**
   * This method cleans up resources
   *
   * @throws Exception
   */
  @Override
  public void close() throws Exception {
    destroy(nativeVm);
    isEvmcLibraryLoaded = false;
    evmcVm = null;
  }
}
