// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc;

/** Exception thrown when the EVMC binding fails to load. */
public class EvmcLoaderException extends Exception {
  public EvmcLoaderException(String message, Throwable cause) {
    super(message, cause);
  }

  public EvmcLoaderException(String message) {
    super(message);
  }
}
