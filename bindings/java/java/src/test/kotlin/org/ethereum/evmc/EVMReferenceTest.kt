// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
package org.ethereum.evmc

import com.fasterxml.jackson.annotation.JsonIgnoreProperties
import com.fasterxml.jackson.core.type.TypeReference
import com.fasterxml.jackson.databind.ObjectMapper
import kotlinx.coroutines.runBlocking
import org.apache.tuweni.bytes.Bytes
import org.apache.tuweni.eth.Address
import org.apache.tuweni.eth.Hash
import org.apache.tuweni.io.Resources
import org.apache.tuweni.junit.BouncyCastleExtension
import org.apache.tuweni.trie.MerklePatriciaTrie
import org.apache.tuweni.units.bigints.UInt256
import org.apache.tuweni.units.ethereum.Gas
import org.apache.tuweni.units.ethereum.Wei
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotEquals
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Assumptions.assumeFalse
import org.junit.jupiter.api.BeforeAll
import org.junit.jupiter.api.extension.ExtendWith
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.Arguments
import org.junit.jupiter.params.provider.MethodSource
import java.io.IOException
import java.io.InputStream
import java.io.UncheckedIOException
import java.util.stream.Collectors
import java.util.stream.Stream
import kotlin.collections.HashMap

const val MAX_CYCLES = 1

@ExtendWith(BouncyCastleExtension::class)
class EVMReferenceTest {

  companion object {

    val mapper = ObjectMapper()

    init {
      mapper.registerModule(EthJsonModule())
    }

    @JvmStatic
    @BeforeAll
    fun checkOS() {
      val osName = System.getProperty("os.name").toLowerCase()
      val isWindows = osName.startsWith("windows")
      assumeFalse(isWindows, "No Windows binaries available")
    }

    @JvmStatic
    @Throws(IOException::class)
    private fun findTests(): Stream<Arguments> {
      val tests = Resources.find("**/*.json").flatMap { url ->
        try {
          url.openConnection().getInputStream().use { input -> prepareTests(input) }
        } catch (e: IOException) {
          throw UncheckedIOException(e)
        }
      }.collect(Collectors.toList())
      val allTests = ArrayList<Arguments>()
      for (i in 1..MAX_CYCLES) {
        allTests.addAll(tests.map {
          Arguments.of(it.get()[0] as String + "-" + i, it.get()[1])
        })
      }
      return allTests.stream()
    }

    @Throws(IOException::class)
    private fun prepareTests(input: InputStream): Stream<Arguments> {

      val typeRef = object : TypeReference<HashMap<String, JsonReferenceTest>>() {}
      val allTests: Map<String, JsonReferenceTest> = mapper.readValue(input, typeRef)
      return allTests
        .entries
        .stream()
        .map { entry ->
          Arguments.of(entry.key, entry.value)
        }
    }
  }

  private val evmcFile: String
  private val evmOneVm: String

  init {
    val osName = System.getProperty("os.name").toLowerCase()
    val isMacOs = osName.startsWith("mac os x")

    if (isMacOs) {
      evmcFile = EVMReferenceTest::class.java.getResource("/libevmc.dylib").file
      evmOneVm = EVMReferenceTest::class.java.getResource("/libevmone.0.5.0.dylib").file
    } else {
      evmcFile = EVMReferenceTest::class.java.getResource("/libevmc.so").file
      evmOneVm = EVMReferenceTest::class.java.getResource("/libevmone.so.0.5.0").file
    }
  }


  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("findTests")
  fun runReferenceTest(testName: String, test: JsonReferenceTest) {
    assertNotNull(testName)
    println(testName)
    val vm = EthereumVirtualMachine(evmcFile, evmOneVm, env = test.env!!, gasPrice = test.exec?.gasPrice!!, options = mapOf(Pair("verbose", "3")))
    test.pre!!.forEach { address, state ->
      runBlocking {
        val tree = MerklePatriciaTrie.storingBytes()
        state.storage!!.forEach { key, value ->
          runBlocking {
            tree.put(key, value)
          }
        }
        val accountState =
          AccountState(state.nonce!!, state.balance!!, Hash.fromBytes(tree.rootHash()), Hash.hash(state.code!!))
        vm.storeAccount(address, accountState)
      }
    }

    vm.start()
    try {
      val result = vm.execute(
        test.exec?.origin!!,
        test.exec?.address!!,
        test.exec?.value!!,
        test.exec?.code!!,
        test.exec?.data!!,
        test.exec?.gas!!
      )
      if (test.post == null) {
        assertNotEquals(EVMExecutionStatusCode.EVMC_SUCCESS, result.statusCode)
        if (testName.contains("JumpDest", true)
          || testName.contains("OutsideBoundary", true)
          || testName.contains("outOfBoundary", true)
          || testName.startsWith("DynamicJump_valueUnderflow")
          || testName.startsWith("jumpiToUintmaxPlus1")
          || testName.startsWith("jumpToUintmaxPlus1")
          || testName.startsWith("DynamicJumpi0")
          || testName.startsWith("DynamicJumpJD_DependsOnJumps0")
          || testName.startsWith("jumpHigh")
          || testName.startsWith("bad_indirect_jump2")
          || testName.startsWith("DynamicJumpPathologicalTest1")
          || testName.startsWith("jumpToUint64maxPlus1")
          || testName.startsWith("jumpiToUint64maxPlus1")
          || testName.startsWith("jumpi0")
          || testName.startsWith("DynamicJumpPathologicalTest3")
          || testName.startsWith("DynamicJumpPathologicalTest2")
          || testName.startsWith("jump1")
          || testName.startsWith("bad_indirect_jump1")
          || testName.startsWith("BlockNumberDynamicJumpi0")
          || testName.startsWith("gasOverFlow")
          || testName.startsWith("DynamicJump1")
          || testName.startsWith("BlockNumberDynamicJump1")
          || testName.startsWith("JDfromStorageDynamicJump1")
          || testName.startsWith("JDfromStorageDynamicJumpi0")
        ) {
          assertEquals(EVMExecutionStatusCode.EVMC_BAD_JUMP_DESTINATION, result.statusCode)
        } else if (testName.contains("underflow",true)
          || testName.startsWith("swap2error")
          || testName.startsWith("dup2error")
          || testName.startsWith("pop1")
          || testName.startsWith("jumpOntoJump")
          || testName.startsWith("swapAt52becameMstore")
          || testName.startsWith("stack_loop")
          || testName.startsWith("201503110206PYTHON")
          || testName.startsWith("201503112218PYTHON")
          || testName.startsWith("201503110219PYTHON")
          || testName.startsWith("201503102320PYTHON")
        ) {
          assertEquals(EVMExecutionStatusCode.EVMC_STACK_UNDERFLOW, result.statusCode)
        } else if (testName.contains("outofgas", true)
          || testName.contains("TooHigh", true)
          || testName.contains("MemExp", true)
          || testName.contains("return1", true)
          || testName.startsWith("sha3_bigOffset")
          || testName.startsWith("sha3_3")
          || testName.startsWith("sha3_4")
          || testName.startsWith("sha3_5")
          || testName.startsWith("sha3_6")
          || testName.startsWith("sha3_bigSize")
          || testName.startsWith("ackermann33")
        ) {
          assertEquals(EVMExecutionStatusCode.EVMC_OUT_OF_GAS, result.statusCode)
        } else if (testName.contains("stacklimit", true)) {
          assertEquals(EVMExecutionStatusCode.EVMC_STACK_OVERFLOW, result.statusCode)
        } else {
          println(result.statusCode)
          TODO()
        }
      } else {
        test.post!!.forEach { address, state ->
          runBlocking {
            assertTrue(vm.accounts.containsKey(address))
            val accountState = vm.accounts.get(address)
            val balance = accountState?.balance?.add(result.hostContext.balanceChanges.get(address) ?: Wei.valueOf(0)) ?: Wei.valueOf(0)
            assertEquals(state.balance, balance)
            assertEquals(state.nonce, accountState!!.nonce)
          }
        }
      }
    } finally {
      vm.stop()
    }
  }
}

@JsonIgnoreProperties(ignoreUnknown = true)
data class Env(
  var currentCoinbase: Address? = null,
  var currentDifficulty: Bytes? = null,
  var currentGasLimit: Gas? = null,
  var currentNumber: Bytes? = null,
  var currentTimestamp: Bytes? = null
)

@JsonIgnoreProperties(ignoreUnknown = true)
data class Exec(
  var address: Address? = null,
  var caller: Address? = null,
  var code: Bytes? = null,
  var data: Bytes? = null,
  var gas: Gas? = null,
  var gasPrice: Bytes? = null,
  var origin: Address? = null,
  var value: Bytes? = null
)

@JsonIgnoreProperties(ignoreUnknown = true)
data class JsonAccountState(
  var balance: Wei? = null,
  var code: Bytes? = null,
  var nonce: UInt256? = null,
  var storage: Map<Bytes, Bytes>? = null
)


@JsonIgnoreProperties(ignoreUnknown = true)
data class JsonReferenceTest(
  var env: Env? = null,
  var exec: Exec? = null,
  var gas: Gas? = null,
  var logs: Bytes? = null,
  var out: Bytes? = null,
  var post: Map<Address, JsonAccountState>? = null,
  var pre: Map<Address, JsonAccountState>? = null
)
