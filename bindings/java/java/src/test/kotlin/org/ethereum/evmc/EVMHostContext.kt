// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc

import kotlinx.coroutines.runBlocking
import org.apache.tuweni.bytes.Bytes
import org.apache.tuweni.bytes.Bytes32
import org.apache.tuweni.eth.Address
import org.apache.tuweni.eth.Hash
import org.apache.tuweni.eth.Log
import org.apache.tuweni.trie.MerklePatriciaTrie
import org.apache.tuweni.units.bigints.UInt256
import org.apache.tuweni.units.ethereum.Gas
import org.apache.tuweni.units.ethereum.Wei
import org.slf4j.LoggerFactory
import java.nio.ByteBuffer

/**
 * EVM context that records changes to the world state, so they can be applied atomically.
 */
class EVMHostContext(
        val vm: EthereumVirtualMachine,
        val depth: Int,
        val sender: Address,
        val destination: Address,
        val value: Bytes,
        val code: Bytes,
        val gas: Gas
) : HostContext {

    companion object {
        private val logger = LoggerFactory.getLogger(EVMHostContext::class.java)
    }

    private val buffers = mutableListOf<ByteBuffer>()

    var blockHash: Hash? = null
    val accountChanges = HashMap<Address, HashMap<Bytes, Bytes>>()
    val logs = mutableListOf<Log>()
    val accountsToDestroy = mutableListOf<Address>()
    val balanceChanges = HashMap<Address, Wei>()

    private fun alloc(size: Int): ByteBuffer {
        val buffer = ByteBuffer.allocateDirect(size)
        buffers.add(buffer)
        return buffer
    }

    /**
     * Check account existence function.
     *
     *
     * This function is used by the VM to check if there exists an account at given address.
     *
     * @param address The address of the account the query is about.
     * @return true if exists, false otherwise.
     */
    override fun accountExists(bytes: ByteArray): Boolean = runBlocking {
        logger.trace("Entering accountExists")
        val address = Address.fromBytes(Bytes.wrap(bytes))
        accountChanges.containsKey(address)
    }

    /**
     * Get storage function.
     *
     *
     * This function is used by a VM to query the given account storage entry.
     *
     * @param address The address of the account.
     * @param key The index of the account's storage entry.
     * @return The storage value at the given storage key or null bytes if the account does not exist.
     */
    override fun getStorage(addressBytes: ByteArray, keyBytes: ByteArray): ByteBuffer = runBlocking {
        logger.trace("Entering getStorage")
        val address = Address.fromBytes(Bytes.wrap(addressBytes))
        val key = Bytes32.wrap(keyBytes)
        val value = accountChanges[address]?.get(key)
        logger.info("Found value $value")
        return@runBlocking if (value == null) {
            alloc(32).put(ByteArray(32))
        } else {
            alloc(value.size()).put(value.toArrayUnsafe())
        }
    }

    /**
     * Set storage function.
     *
     *
     * This function is used by a VM to update the given account storage entry. The VM MUST make
     * sure that the account exists. This requirement is only a formality because VM implementations
     * only modify storage of the account of the current execution context (i.e. referenced by
     * evmc_message::destination).
     *
     * @param address The address of the account.
     * @param key The index of the storage entry.
     * @param value The value to be stored.
     * @return The effect on the storage item:
     * The value of a storage item has been left unchanged: 0 -> 0 and X -> X.
     * EVMC_STORAGE_UNCHANGED = 0,
     * The value of a storage item has been modified: X -> Y.
     * EVMC_STORAGE_MODIFIED = 1,
     * A storage item has been modified after being modified before: X -> Y -> Z.
     * EVMC_STORAGE_MODIFIED_AGAIN = 2,
     * A new storage item has been added: 0 -> X.
     * EVMC_STORAGE_ADDED = 3,
     * A storage item has been deleted: X -> 0.
     * EVMC_STORAGE_DELETED = 4
     */
    override fun setStorage(addressBytes: ByteArray, keyBytes: ByteArray, valueBytes: ByteArray): Int {
        val key = Bytes.wrap(keyBytes)
        val value = Bytes.wrap(valueBytes)
        val address = Address.fromBytes(Bytes.wrap(addressBytes))
        logger.trace("Entering setStorage {} {} {}", address, key, value)
        var newAccount = false
        accountChanges.computeIfAbsent(address) {
            newAccount = true
            HashMap()
        }
        val map = accountChanges[address]!!
        val oldValue = map.get(key)
        val storageAdded = newAccount || oldValue == null
        val storageWasModifiedBefore = map.containsKey(key)
        val storageModified = !value.equals(oldValue)
        if (value.size() == 0) {
            map.remove(key)
            return 4
        }
        map.put(key, value)
        if (storageModified) {
            if (storageAdded) {
                return 3
            }
            if (storageWasModifiedBefore) {
                return 2
            }
            return 1
        }
        return 0
    }

    /**
     * Get balance function.
     *
     *
     * This function is used by a VM to query the balance of the given account.
     *
     * @param address The address of the account.
     * @return The balance of the given account or 0 if the account does not exist.
     */
    override fun getBalance(addressBytes: ByteArray): ByteBuffer = runBlocking {
        logger.trace("Entering getBalance")
        val address = Address.fromBytes(Bytes.wrap(addressBytes))
        val balance = balanceChanges[address]
        balance?.let {
            return@runBlocking alloc(32).put(it.toBytes().toArrayUnsafe())
        }
        val response = alloc(0)
        response
    }

    /**
     * Get code size function.
     *
     *
     * This function is used by a VM to get the size of the code stored in the account at the given
     * address.
     *
     * @param address The address of the account.
     * @return The size of the code in the account or 0 if the account does not exist.
     */
    override fun getCodeSize(address: ByteArray): Int = runBlocking {
        logger.trace("Entering getCodeSize")
        val code = vm.accountCodes[Address.fromBytes(Bytes.wrap(address))]
        code?.size() ?: 0
    }

    /**
     * Get code hash function.
     *
     *
     * This function is used by a VM to get the keccak256 hash of the code stored in the account at
     * the given address. For existing accounts not having a code, this function returns keccak256
     * hash of empty data.
     *
     * @param address The address of the account.
     * @return The hash of the code in the account or null bytes if the account does not exist.
     */
    override fun getCodeHash(address: ByteArray): ByteBuffer = runBlocking {
        logger.trace("Entering getCodeHash")
        val code = vm.accountCodes[Address.fromBytes(Bytes.wrap(address))]
        val response = alloc(32)
        code?.let { response.put(Hash.hash(it).toArrayUnsafe()) }
        response
    }

    /**
     * Copy code function.
     *
     *
     * This function is used by an EVM to request a copy of the code of the given account to the
     * memory buffer provided by the EVM. The Client MUST copy the requested code, starting with the
     * given offset, to the provided memory buffer up to the size of the buffer or the size of the
     * code, whichever is smaller.
     *
     * @param address The address of the account.
     * @return A copy of the requested code.
     */
    override fun getCode(address: ByteArray): ByteBuffer = runBlocking {
        logger.trace("Entering getCode")
        val code = vm.accountCodes[Address.fromBytes(Bytes.wrap(address))]
        val response = alloc(code?.size() ?: 0)
        code?.let { response.put(it.toArrayUnsafe()) }
        response
    }

    /**
     * Selfdestruct function.
     *
     *
     * This function is used by an EVM to SELFDESTRUCT given contract. The execution of the
     * contract will not be stopped, that is up to the EVM.
     *
     * @param address The address of the contract to be selfdestructed.
     * @param beneficiary The address where the remaining ETH is going to be transferred.
     */
    override fun selfdestruct(address: ByteArray, beneficiary: ByteArray): Unit = runBlocking {
        logger.trace("Entering selfdestruct")
        val addr = Address.fromBytes(Bytes.wrap(address))
        accountsToDestroy.add(addr)
        val account = vm.accounts.get(addr)
        val beneficiaryAddress = Address.fromBytes(Bytes.wrap(beneficiary))
        vm.accounts.computeIfAbsent(beneficiaryAddress) { AccountState(UInt256.ZERO, Wei.valueOf(0), Hash.fromBytes(MerklePatriciaTrie.storingBytes().rootHash()), Hash.hash(Bytes.EMPTY)) }
        account?.apply {
            val balance = balanceChanges.putIfAbsent(beneficiaryAddress, account.balance)
            balance?.let {
                balanceChanges[beneficiaryAddress] = it.add(account.balance)
            }
        }
        logger.trace("Done selfdestruct")
        return@runBlocking
    }

    /**
     * This function supports EVM calls.
     *
     * @param msg The call parameters.
     * @return The result of the call.
     */
    override fun call(msg: ByteBuffer): ByteBuffer {
        logger.trace("Entering call")
        val evmMessage = EVMMessage.fromBytes(Bytes.wrapByteBuffer(msg))
        val hostContext = EVMHostContext(vm, depth + 1, evmMessage.sender, evmMessage.destination, evmMessage.value, Bytes.EMPTY, evmMessage.gas)
        val result = vm.executeInternal(
                evmMessage.sender,
                evmMessage.destination,
                evmMessage.value,
                Bytes.EMPTY,
                evmMessage.inputData,
                evmMessage.inputDataSize,
                evmMessage.gas,
                depth = depth + 1,
                hostContext = hostContext
        )
        return result
    }

    /**
     * Get transaction context function.
     *
     *
     * This function is used by an EVM to retrieve the transaction and block context.
     *
     * @return The transaction context:
     *
     */
    override fun getTxContext(): ByteBuffer {
        logger.trace("Entering getTxContext")

        return alloc(160).put(Bytes.concatenate(Bytes32.leftPad(vm.gasPrice),
                sender, vm.env.currentCoinbase!!, Bytes.ofUnsignedLong(vm.env.currentNumber!!.toLong()),
                Bytes.ofUnsignedLong(vm.env.currentTimestamp!!.toLong()),
                Bytes.ofUnsignedLong(vm.env.currentGasLimit!!.toLong()),
                UInt256.fromBytes(vm.env.currentDifficulty!!).toBytes(),
                UInt256.ONE.toBytes()
        ).toArrayUnsafe())
    }

    /**
     * Get block hash function.
     *
     *
     * This function is used by a VM to query the hash of the header of the given block. If the
     * information about the requested block is not available, then this is signalled by returning
     * null bytes.
     *
     * @param number The block number.
     * @return The block hash or null bytes if the information about the block is not available.
     */
    override fun getBlockHash(number: Long): ByteBuffer {
        logger.trace("Entering getBlockHash")
        return alloc(32).put(blockHash!!.toArrayUnsafe())
    }

    /**
     * Log function.
     *
     *
     * This function is used by an EVM to inform about a LOG that happened during an EVM bytecode
     * execution.
     *
     * @param address The address of the contract that generated the log.
     * @param data The unindexed data attached to the log.
     * @param dataSize The length of the data.
     * @param topics The the array of topics attached to the log.
     * @param topicCount The number of the topics. Valid values are between 0 and 4 inclusively.
     */
    override fun emitLog(address: ByteArray, data: ByteArray, dataSize: Int, topics: Array<ByteArray>, topicCount: Int) {
        logger.trace("Entering emitLog")
        logs.add(Log(Address.fromBytes(Bytes.wrap(address)), Bytes.wrap(data), topics.map { Bytes32.wrap(it) }))
    }

    override fun close() {
        buffers.forEach {
            it.clear()
        }
        buffers.clear()
    }
}
