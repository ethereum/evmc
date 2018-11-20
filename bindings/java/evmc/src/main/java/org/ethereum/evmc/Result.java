package org.ethereum.evmc;

public class Result {
    long gasLeft;
    byte[] output;
    Address createAddress;

    public void dispose() {
    }
}
