package org.ethereum.evmc;

import static java.util.Objects.requireNonNull;

import java.util.Objects;
import org.apache.tuweni.bytes.Bytes;
import org.apache.tuweni.eth.Hash;
import org.apache.tuweni.rlp.RLP;
import org.apache.tuweni.units.bigints.UInt256;
import org.apache.tuweni.units.ethereum.Wei;

/** State of an account as stored on chain. */
public final class AccountState {

  private static final int DEFAULT_VERSION = 0;

  private final UInt256 nonce;

  private final Wei balance;

  private final Hash storageRoot;

  private final Hash codeHash;
  private final int version;

  /**
   * Reads an account state from a RLP representation.
   *
   * @param bytes the bytes of the serialized acount state.
   * @return the AccountState read from the bytes.
   */
  public static org.apache.tuweni.eth.AccountState fromBytes(Bytes bytes) {
    requireNonNull(bytes);
    return RLP.decodeList(
        bytes,
        reader -> {
          return new org.apache.tuweni.eth.AccountState(
              reader.readUInt256(),
              Wei.valueOf(reader.readUInt256()),
              Hash.fromBytes(reader.readValue()),
              Hash.fromBytes(reader.readValue()),
              reader.isComplete() ? DEFAULT_VERSION : reader.readInt());
        });
  }

  /**
   * Default constructor for account state without a version.
   *
   * @param nonce the nonce of the account.
   * @param balance the balance of the account.
   * @param storageRoot the root hash of the storage.
   * @param codeHash hash of the code of the account.
   */
  public AccountState(UInt256 nonce, Wei balance, Hash storageRoot, Hash codeHash) {
    this(nonce, balance, storageRoot, codeHash, DEFAULT_VERSION);
  }

  /**
   * Default constructor for account state.
   *
   * @param nonce the nonce of the account.
   * @param balance the balance of the account.
   * @param storageRoot the root hash of the storage.
   * @param codeHash hash of the code of the account.
   * @param version the version of the account state.
   */
  public AccountState(UInt256 nonce, Wei balance, Hash storageRoot, Hash codeHash, int version) {
    this.nonce = nonce;
    this.balance = balance;
    this.storageRoot = storageRoot;
    this.codeHash = codeHash;
    this.version = version;
  }

  public Bytes toBytes() {
    return RLP.encodeList(
        writer -> {
          writer.writeValue(nonce.toMinimalBytes());
          writer.writeValue(balance.toMinimalBytes());
          writer.writeValue(storageRoot);
          writer.writeValue(codeHash);
          if (version != DEFAULT_VERSION) {
            writer.writeInt(version);
          }
        });
  }

  public Hash getCodeHash() {
    return codeHash;
  }

  public Hash getStorageRoot() {
    return storageRoot;
  }

  public int getVersion() {
    return version;
  }

  public UInt256 getNonce() {
    return nonce;
  }

  public Wei getBalance() {
    return balance;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    AccountState that = (AccountState) o;
    return version == that.version
        && Objects.equals(nonce, that.nonce)
        && Objects.equals(balance, that.balance)
        && Objects.equals(storageRoot, that.storageRoot)
        && Objects.equals(codeHash, that.codeHash);
  }

  @Override
  public int hashCode() {
    return Objects.hash(nonce, balance, storageRoot, codeHash, version);
  }
}
