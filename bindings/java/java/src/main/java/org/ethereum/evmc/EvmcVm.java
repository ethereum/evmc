// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * The Java interface to the evm instance.
 *
 * <p>Defines the Java methods capable of accessing the evm implementation.
 */
public final class EvmcVm implements AutoCloseable {
  private static final Throwable evmcLoadingError;
  private ByteBuffer nativeVm;

  // Load the dynamic library containing the JNI bindings to EVMC.
  static {
    Throwable error = null;

    // First try loading from global path.
    try {
      System.loadLibrary("libevmc-java");
    } catch (UnsatisfiedLinkError globalLoadingError) {
      String extension = null;
      String os = System.getProperty("os.name").toLowerCase();
      if (os.contains("win")) {
        extension = "dll";
      } else if (os.contains("nix") || os.contains("nux") || os.contains("aix")) {
        extension = "so";
      } else if (os.contains("mac") || os.contains("darwin")) {
        extension = "dylib";
      } else {
        // Give up, because we are unsure what system we are running on.
        error = globalLoadingError;
      }

      // Try loading the binding from the package.
      if (extension != null) {
        try {
          Path evmcLib = Files.createTempFile("libevmc-java", extension);
          Files.copy(
              EvmcVm.class.getResourceAsStream("/libevmc-java." + extension),
              evmcLib,
              StandardCopyOption.REPLACE_EXISTING);
          evmcLib.toFile().deleteOnExit();
          // We are somewhat certain about the file, try loading it.
          try {
            System.load(evmcLib.toAbsolutePath().toString());
          } catch (UnsatisfiedLinkError packageLoadingError) {
            error = packageLoadingError;
          }
        } catch (IOException packageCreationError) {
          error = packageCreationError;
        }
      }
    }
    evmcLoadingError = error;
  }

  /**
   * Returns true if the native library was loaded successfully and EVMC capabilities are available.
   *
   * @return true if the library is available
   */
  public static boolean isAvailable() {
    return evmcLoadingError == null;
  }

  /**
   * This method loads the specified evm shared library and loads/initializes the jni bindings.
   *
   * @param filename /path/filename of the evm shared object
   * @throws org.ethereum.evmc.EvmcLoaderException
   */
  public static EvmcVm create(String filename) throws EvmcLoaderException {
    if (!isAvailable()) {
      throw new EvmcLoaderException("EVMC JNI binding library failed to load", evmcLoadingError);
    }
    return new EvmcVm(filename);
  }

  private EvmcVm(String filename) throws EvmcLoaderException {
    nativeVm = load_and_create(filename);
  }

  /**
   * This method loads the specified EVM implementation and returns its pointer.
   *
   * @param filename Path to the dynamic object representing the EVM implementation
   * @return Internal object pointer.
   * @throws org.ethereum.evmc.EvmcLoaderException
   */
  private static native ByteBuffer load_and_create(String filename) throws EvmcLoaderException;

  /**
   * EVMC ABI version implemented by the VM instance.
   *
   * <p>Can be used to detect ABI incompatibilities. The EVMC ABI version represented by this file
   * is in ::EVMC_ABI_VERSION.
   */
  public static native int abi_version();

  /**
   * The name of the EVMC VM implementation.
   *
   * <p>It MUST be a NULL-terminated not empty string. The content MUST be UTF-8 encoded (this
   * implies ASCII encoding is also allowed).
   */
  private static native String name(ByteBuffer nativeVm);

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
  private static native String version(ByteBuffer nativeVm);

  /** Function is a wrapper around native version(). */
  public String version() {
    return version(nativeVm);
  }

  /**
   * Function to destroy the VM instance.
   *
   * <p>This is a mandatory method and MUST NOT be set to NULL.
   */
  private static native void destroy(ByteBuffer nativeVm);

  /**
   * Function to execute a code by the VM instance.
   *
   * <p>This is a mandatory method and MUST NOT be set to NULL.
   */
  private static native ByteBuffer execute(
      ByteBuffer nativeVm, HostContext context, int rev, ByteBuffer msg, ByteBuffer code);

  /**
   * Function is a wrapper around native execute.
   *
   * <p>This allows the context to managed in one method
   */
  public synchronized ByteBuffer execute(
      HostContext context, int rev, ByteBuffer msg, ByteBuffer code) {
    return execute(nativeVm, context, rev, msg, code);
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
  private static native int get_capabilities(ByteBuffer nativeVm);

  /** Function is a wrapper around native get_capabilities(). */
  public int get_capabilities() {
    return get_capabilities(nativeVm);
  }

  /**
   * Function that modifies VM's options.
   *
   * <p>If the VM does not support this feature the pointer can be NULL.
   */
  private static native int set_option(ByteBuffer nativeVm, String name, String value);

  /** Function is a wrapper around native set_option(). */
  public int set_option(String name, String value) {
    return set_option(nativeVm, name, value);
  }

  /** This method cleans up resources. */
  @Override
  public void close() {
    destroy(nativeVm);
  }
}
