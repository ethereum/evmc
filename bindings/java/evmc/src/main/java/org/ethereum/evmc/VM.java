package org.ethereum.evmc;

enum Revision {
    frontier, homestead, tangerineWhistle, spuriousDragon, constantinople
}

interface VM {
    public Result execute(Host host, Revision rev, Message msg, byte[] code);
}
