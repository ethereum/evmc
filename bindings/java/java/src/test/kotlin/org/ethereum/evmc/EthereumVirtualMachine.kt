// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc

import org.apache.tuweni.bytes.Bytes
import org.apache.tuweni.bytes.Bytes32
import org.apache.tuweni.eth.Address
import org.apache.tuweni.units.ethereum.Gas
import org.slf4j.LoggerFactory
import java.nio.ByteBuffer
import java.nio.ByteOrder



/**
 * Types of EVM calls
 */
enum class CallKind(val number: Int) {
  EVMC_CALL(0),
  EVMC_DELEGATECALL(1),
  EVMC_CALLCODE(2),
  EVMC_CREATE(3),
  EVMC_CREATE2(4)
}

/**
 * EVM execution status codes
 */
enum class EVMExecutionStatusCode(val number: Int) {
  EVMC_SUCCESS(0),
  EVMC_FAILURE(1),
  EVMC_REVERT(2),
  EVMC_OUT_OF_GAS(3),
  EVMC_INVALID_INSTRUCTION(4),
  EVMC_UNDEFINED_INSTRUCTION(5),
  EVMC_STACK_OVERFLOW(6),
  EVMC_STACK_UNDERFLOW(7),
  EVMC_BAD_JUMP_DESTINATION(8),
  EVMC_INVALID_MEMORY_ACCESS(9),
  EVMC_CALL_DEPTH_EXCEEDED(10),
  EVMC_STATIC_MODE_VIOLATION(11),
  EVMC_PRECOMPILE_FAILURE(12),
  EVMC_CONTRACT_VALIDATION_FAILURE(13),
  EVMC_ARGUMENT_OUT_OF_RANGE(14),
  EVMC_WASM_UNREACHABLE_INSTRUCTION(15),
  EVMC_WASM_TRAP(16),
  EVMC_INTERNAL_ERROR(-1),
  EVMC_REJECTED(-2),
  EVMC_OUT_OF_MEMORY(-3);
}

/**
 * Finds a code matching a number, or throw an exception if no matching code exists.
 * @param code the number to match
 * @return the execution code
 */
fun fromCode(code: Int): EVMExecutionStatusCode = EVMExecutionStatusCode.values().first {
  code == it.number
}

/**
 * Known hard fork revisions to execute against.
 */
enum class HardFork(val number: Int) {
  EVMC_FRONTIER(0),
  EVMC_HOMESTEAD(1),
  EVMC_TANGERINE_WHISTLE(2),
  EVMC_SPURIOUS_DRAGON(3),
  EVMC_BYZANTIUM(4),
  EVMC_CONSTANTINOPLE(5),
  EVMC_PETERSBURG(6),
  EVMC_ISTANBUL(7),
  EVMC_BERLIN(8),
  EVMC_MAX_REVISION(8)
}

/**
 * Result of EVM execution
 * @param statusCode the execution result status
 * @param gasLeft how much gas is left
 * @param hostContext the context of changes
 */
data class EVMResult(
  val statusCode: EVMExecutionStatusCode,
  val gasLeft: Long,
  val hostContext: EVMHostContext
) {
  companion object {
    fun fromBytes(bytes: Bytes, hostContext: EVMHostContext): EVMResult {
      return EVMResult(fromCode(bytes.getInt(0)), bytes.getLong(8), hostContext)
    }
  }

  fun toBytes() =
    Bytes32.rightPad(Bytes.concatenate(Bytes.ofUnsignedInt(statusCode.number.toLong()), Bytes.ofUnsignedLong(gasLeft)))
}

/**
 * Message sent to the EVM for execution
 */
internal data class EVMMessage(
  val kind: Int,
  val flags: Int,
  val depth: Int = 0,
  val gas: Gas,
  val destination: Address,
  val sender: Address,
  val inputData: Long,
  val inputDataSize: Int,
  val value: Bytes,
  val createSalt: Bytes32 = Bytes32.ZERO
) {

  companion object {
    fun fromBytes(message: Bytes): EVMMessage {
      return EVMMessage(
        message.getInt(0),
        message.getInt(4),
        message.getInt(8),
        Gas.valueOf(message.getLong(16)),
        Address.fromBytes(message.slice(24, 20)),
        Address.fromBytes(message.slice(44, 20)),
           message.getLong(64),
              message.getInt(72),
        message.slice(76, 32),
        Bytes32.wrap(message.slice(108, 32))
      )
    }
  }

  fun toByteBuffer(): ByteBuffer {

    return ByteBuffer.allocateDirect(
      4 + 4 + 4 + 4 + 8 + 20 + 20 + 8 +
        4 + 32 + 32
    ).order(ByteOrder.nativeOrder())
      .putInt(kind)
      .putInt(flags)
      .putInt(depth)
            .putInt(0)// padding?
      .putLong(gas.toLong())
      .put(destination.toArrayUnsafe())
      .put(sender.toArrayUnsafe())
      .putLong(inputData)
      .putInt(inputDataSize)
      .put(Bytes32.leftPad(value).toArrayUnsafe())
      .put(createSalt.toArrayUnsafe())
  }
}

/**
 * An Ethereum Virtual Machine.
 *
 * @param repository the blockchain repository
 * @param evmcFile the full path of the shared library for EVMc
 * @param vmFile the full path to an EVM library compatible with EVMc, such as evmone or hera
 * @param options the options to set on the EVM, specific to the library
 */
class EthereumVirtualMachine(
        private val evmcFile: String,
        private val vmFile: String,
        private val options: Map<String, String> = mapOf(),
        val accounts: HashMap<Address, AccountState> = HashMap(),
        val accountCodes: HashMap<Address, Bytes> = HashMap(),
        val env: Env,
        val gasPrice: Bytes
) {

  companion object {
    private val logger = LoggerFactory.getLogger(EthereumVirtualMachine::class.java)
  }

  private var vm: EvmcVm? = null

  private fun vm() = vm!!

  /**
   * Start the EVM
   */
  fun start() {
    vm = EvmcVm.create(evmcFile, vmFile)
    options.forEach { (k, v) ->
      vm().set_option(k, v)
    }
  }

  /**
   * Provides the version of the EVM
   *
   * @return the version of the underlying EVM library
   */
  fun version(): String = vm().version()

  /**
   * Stop the EVM
   */
  fun stop() {
    vm().close()
  }

  /**
   * Execute an operation in the EVM.
   * @param sender the sender of the transaction
   * @param destination the destination of the transaction
   * @param code the code to execute
   * @param inputData the execution input
   * @param gas the gas available for the operation
   * @param callKind the type of call
   * @param revision the hard fork revision in which to execute
   * @return the result of the execution
   */
  fun execute(
    sender: Address,
    destination: Address,
    value: Bytes,
    code: Bytes,
    inputData: Bytes,
    gas: Gas,
    callKind: CallKind = CallKind.EVMC_CALL,
    revision: HardFork = HardFork.EVMC_BYZANTIUM,
    depth: Int = 0
  ): EVMResult {
    val hostContext = EVMHostContext(this, depth, sender, destination, value, code, gas)
    val inputDataBuffer = ByteBuffer.allocateDirect(inputData.size()).put(inputData.toArrayUnsafe())
    val result =
      executeInternal(sender, destination, value, code, vm().address(inputDataBuffer), inputData.size(), gas, callKind, revision, depth, hostContext)

    return EVMResult.fromBytes(Bytes.wrapByteBuffer(result), hostContext)
  }

  internal fun executeInternal(
    sender: Address,
    destination: Address,
    value: Bytes,
    code: Bytes,
    inputDataAddress: Long,
    inputDataSize: Int,
    gas: Gas,
    callKind: CallKind = CallKind.EVMC_CALL,
    revision: HardFork = HardFork.EVMC_MAX_REVISION,
    depth: Int = 0,
    hostContext: HostContext
  ): ByteBuffer {


    val msg = EVMMessage(callKind.number, 0, depth, gas, destination, sender, inputDataAddress, inputDataSize, value).toByteBuffer()

    val result = vm().execute(
      hostContext,
      revision.number,
      msg,
      ByteBuffer.allocateDirect(code.size()).put(code.toArrayUnsafe()),
      code.size()
    ).order(ByteOrder.nativeOrder())
    return result
  }

  /**
   * Provides the capabilities exposed by the underlying EVM library
   *
   * @return the EVM capabilities
   */
  fun capabilities(): Int = vm()._capabilities

  fun storeAccount(address: Address, accountState: AccountState) {
    accounts.put(address, accountState)
  }
}
