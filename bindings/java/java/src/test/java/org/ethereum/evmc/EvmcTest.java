package org.ethereum.evmc;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

final class EvmcTest {
  private static final String exampleVmPath =
      System.getProperty("user.dir") + "/../c/build/lib/libexample-vm.so";

  @Test
  void testInitCloseDestroy() throws Exception {
    Assertions.assertDoesNotThrow(
        () -> {
          try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {}
        });
  }

  @Test
  void testAbiVersion() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      int abiVersion = vm.abi_version();
      assert (abiVersion > 0);
    }
  }

  @Test
  void testName() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      String name = vm.name();
      assert (name.length() > 0);
      assert (name.equals("example_vm"));
    }
  }

  @Test
  void testVersion() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      String version = vm.version();
      assert (version.length() > 0);
      assert (version.equals("0.0.0"));
    }
  }

  @Test
  void testExecute_returnAddress() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      HostContext context = new TestHostContext();
      int BYZANTIUM = 4;
      int EVMC_CALL = 0;
      int kind = EVMC_CALL;
      char[] sender = "39bf71de1b7d7be3b51\0".toCharArray();
      char[] destination = "53cf77204eEef952e25\0".toCharArray();
      char[] value = "1\0".toCharArray();
      char[] inputData = "hello w\0".toCharArray();
      long gas = 200000;
      int depth = 0;
      ByteBuffer msg =
          new TestMessage(kind, sender, destination, value, inputData, gas, depth).toByteBuffer();

      byte[] code = {0x30, 0x60, 0x00, 0x52, 0x59, 0x60, 0x00, (byte) 0xf3}; // return_address
      ByteBuffer bbcode = ByteBuffer.allocateDirect(code.length).put(code);

      ByteBuffer result =
          vm.execute(context, BYZANTIUM, msg, bbcode, code.length).order(ByteOrder.nativeOrder());
      int statusCode = result.getInt();
      result.getInt(); // padding
      long gasLeft = result.getLong();
      assert (statusCode == 0);
      assert (gasLeft == 0);
    }
  }

  /** Tests callbacks: get_storage_fn & set_storage_fn */
  @Test
  void testExecute_counter() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      HostContext context = new TestHostContext();
      int BYZANTIUM = 4;
      int EVMC_CALL = 0;
      int kind = EVMC_CALL;
      char[] sender = "39bf71de1b7d7be3b51\0".toCharArray();
      char[] destination = "53cf77204eEef952e25\0".toCharArray();
      char[] value = "1\0".toCharArray();
      char[] inputData = "hello w\0".toCharArray();
      long gas = 200000;
      int depth = 0;
      ByteBuffer msg =
          new TestMessage(kind, sender, destination, value, inputData, gas, depth).toByteBuffer();

      byte[] code = {0x60, 0x01, 0x60, 0x00, 0x54, 0x01, 0x60, 0x00, 0x55}; // counter
      ByteBuffer bbcode = ByteBuffer.allocateDirect(code.length).put(code);

      ByteBuffer result =
          vm.execute(context, BYZANTIUM, msg, bbcode, code.length).order(ByteOrder.nativeOrder());
      int statusCode = result.getInt();
      result.getInt(); // padding
      long gasLeft = result.getLong();
      assert (statusCode == 0);
      assert (gasLeft == 0);
    }
  }

  /** Tests callbacks: get_tx_context_fn */
  @Test
  void testExecute_returnBlockNumber() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      HostContext context = new TestHostContext();
      int BYZANTIUM = 4;
      int EVMC_CALL = 0;
      int kind = EVMC_CALL;
      char[] sender = "39bf71de1b7d7be3b51\0".toCharArray();
      char[] destination = "53cf77204eEef952e25\0".toCharArray();
      char[] value = "1\0".toCharArray();
      char[] inputData = "hello w\0".toCharArray();
      long gas = 200000;
      int depth = 0;
      ByteBuffer msg =
          new TestMessage(kind, sender, destination, value, inputData, gas, depth).toByteBuffer();

      byte[] code = {0x43, 0x60, 0x00, 0x52, 0x59, 0x60, 0x00, (byte) 0xf3}; // return_block_number(
      ByteBuffer bbcode = ByteBuffer.allocateDirect(code.length).put(code);

      ByteBuffer result =
          vm.execute(context, BYZANTIUM, msg, bbcode, code.length).order(ByteOrder.nativeOrder());
      int statusCode = result.getInt();
      result.getInt(); // padding
      long gasLeft = result.getLong();
      assert (statusCode == 0);
      assert (gasLeft == gas / 2);
    }
  }

  /** Tests callbacks: get_tx_context_fn & set_storage_fn */
  @Test
  void testExecute_saveReturnBlockNumber() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      HostContext context = new TestHostContext();
      int BYZANTIUM = 4;
      int EVMC_CALL = 0;
      int kind = EVMC_CALL;
      char[] sender = "39bf71de1b7d7be3b51\0".toCharArray();
      char[] destination = "53cf77204eEef952e25\0".toCharArray();
      char[] value = "1\0".toCharArray();
      char[] inputData = "hello w\0".toCharArray();
      long gas = 200000;
      int depth = 0;
      ByteBuffer msg =
          new TestMessage(kind, sender, destination, value, inputData, gas, depth).toByteBuffer();

      byte[] code = {
        0x43, 0x60, 0x00, 0x55, 0x43, 0x60, 0x00, 0x52, 0x59, 0x60, 0x00, (byte) 0xf3
      }; // save_return_block_number
      ByteBuffer bbcode = ByteBuffer.allocateDirect(code.length).put(code);

      ByteBuffer result =
          vm.execute(context, BYZANTIUM, msg, bbcode, code.length).order(ByteOrder.nativeOrder());
      int statusCode = result.getInt();
      result.getInt(); // padding
      long gasLeft = result.getLong();
      assert (statusCode == 0);
      assert (gasLeft == gas / 2);
    }
  }

  /** Tests callbacks: call_fn */
  @Test
  void testExecute_makeCall() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      HostContext context = new TestHostContext();
      int BYZANTIUM = 4;
      int EVMC_CALL = 0;
      int kind = EVMC_CALL;
      char[] sender = "39bf71de1b7d7be3b51\0".toCharArray();
      char[] destination = "53cf77204eEef952e25\0".toCharArray();
      char[] value = "1\0".toCharArray();
      char[] inputData = "hello w\0".toCharArray();
      long gas = 200000;
      int depth = 0;
      ByteBuffer msg =
          new TestMessage(kind, sender, destination, value, inputData, gas, depth).toByteBuffer();
      byte[] code = {
        0x60,
        0x00,
        (byte) 0x80,
        (byte) 0x80,
        (byte) 0x80,
        (byte) 0x80,
        (byte) 0x80,
        (byte) 0x80,
        (byte) 0xf1
      }; // make_a_call(
      ByteBuffer bbcode = ByteBuffer.allocateDirect(code.length).put(code);

      ByteBuffer result =
          vm.execute(context, BYZANTIUM, msg, bbcode, code.length).order(ByteOrder.nativeOrder());
      int statusCode = result.getInt();
      result.getInt(); // padding
      long gasLeft = result.getLong();
      assert (statusCode == 0);
      assert (gasLeft == 0); // gas - gas / 64);
    }
  }

  @Test
  void testExecute_EVMC_CREATE() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      HostContext context = new TestHostContext();
      int BYZANTIUM = 4;
      int EVMC_CREATE = 3;
      int kind = EVMC_CREATE;
      char[] sender = "39bf71de1b7d7be3b51\\0".toCharArray();
      char[] destination = "53cf77204eEef952e25\0".toCharArray();
      char[] value = "1\0".toCharArray();
      char[] inputData = "hello w\0".toCharArray();
      long gas = 200000;
      int depth = 0;
      ByteBuffer msg =
          new TestMessage(kind, sender, destination, value, inputData, gas, depth).toByteBuffer();
      byte[] code = {0x00};
      ByteBuffer bbcode = ByteBuffer.allocateDirect(code.length).put(code);

      ByteBuffer result =
          vm.execute(context, BYZANTIUM, msg, bbcode, code.length).order(ByteOrder.nativeOrder());
      int statusCode = result.getInt();
      result.getInt(); // padding
      long gasLeft = result.getLong();
      assert (statusCode == 0);
      assert (gasLeft == gas / 10);
    }
  }

  @Test
  void testGetCapabilities() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      int capabilities = vm.get_capabilities();
      assert (capabilities > 0);
    }
  }

  @Test
  void testSetOption() throws Exception {
    try (EvmcVm vm = EvmcVm.create(exampleVmPath)) {
      int result = vm.set_option("verbose", "1");
      assert (result == 0);
    }
  }
}
