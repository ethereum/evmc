package org.ethereum.evmc;

/**
 * A class representing 256-bit hashes and big-endian integers.
 */
class Hash {
}

/**
 * A class representing 160-bit address.
 */
class Address {

}

enum CallKind {
    call, delegatecall, callcode, create, create2
}

public class Message {
    CallKind kind;
    int flags;
    int depth;
    long gas;
    Address destination;
    Address sender;
    byte[] input;
    Hash value;
    Hash create2Salt;
}
