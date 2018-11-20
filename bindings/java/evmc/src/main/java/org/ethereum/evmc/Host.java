package org.ethereum.evmc;

enum StorageStatus {
    unchanged, modified, modifiedAgain, added, deleted
}

class TxContext {
    public Hash txGasPrice;
    public Address txOrigin;
    public Address blockCoinbase;
    public long blockNumber;
    public long blockTimestamp;
    public long blockGasLimit;
    public Hash blockDifficulty;
}

interface Host {
    Result call(Message msg);

    Hash getStorage(Address addr, Hash key);

    StorageStatus setStorage(Address addr, Hash key);

    Hash getBalance(Address addr);

    TxContext getTxContext();
}
