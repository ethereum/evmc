package org.ethereum.evmc;

public enum StorageStatus {
  /** The value of a storage item has been left unchanged: 0 -> 0 and X -> X. */
  EVMC_STORAGE_UNCHANGED(0),

  /** The value of a storage item has been modified: X -> Y. */
  EVMC_STORAGE_MODIFIED(1),

  /** A storage item has been modified after being modified before: X -> Y -> Z. */
  EVMC_STORAGE_MODIFIED_AGAIN(2),

  /** A new storage item has been added: 0 -> X. */
  EVMC_STORAGE_ADDED(3),

  /** A storage item has been deleted: X -> 0. */
  EVMC_STORAGE_DELETED(4);

  public final int code;

  StorageStatus(int code) {
    this.code = code;
  }
}
