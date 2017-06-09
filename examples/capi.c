#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "evm.h"


struct evm_uint256be balance(struct evm_env* env,
                             const struct evm_uint160be* address)
{
    struct evm_uint256be ret = {.bytes = {1, 2, 3, 4}};
    return ret;
}

struct evm_uint160be address(struct evm_env* env)
{
    struct evm_uint160be ret = {.bytes = {1, 2, 3, 4}};
    return ret;
}

static void print_address(const struct evm_uint160be* address)
{
    int i = 0;
    for (i = 0; i < sizeof(address->bytes); ++i)
        printf("%x", address->bytes[i] & 0xff);
}

static void query(union evm_variant* result,
                  struct evm_env* env,
                  enum evm_query_key key,
                  const struct evm_uint160be* address) {
    printf("EVM-C: QUERY %d\n", key);
    switch (key) {
    case EVM_CODE_BY_ADDRESS:
        result->data = NULL;
        result->data_size = 0;
        break;

    case EVM_BALANCE:
        result->uint256be = balance(env, address);
        break;

    case EVM_ACCOUNT_EXISTS:
        result->int64 = 0;
        break;

    default:
        result->int64 = 0;
    }
}

static void get_storage(struct evm_uint256be* result,
                        struct evm_env* env,
                        const struct evm_uint160be* address,
                        const struct evm_uint256be* key)
{
    printf("EVM-C: SLOAD @");
    print_address(address);
    printf("\n");
}

static void set_storage(struct evm_env* env,
                        const struct evm_uint160be* address,
                        const struct evm_uint256be* key,
                        const struct evm_uint256be* value)
{
    printf("EVM-C: SSTORE @");
    print_address(address);
    printf("\n");
}

static void selfdestruct(struct evm_env* env,
                         const struct evm_uint160be* address,
                         const struct evm_uint160be* beneficiary)
{
    printf("EVM-C: SELFDESTRUCT ");
    print_address(address);
    printf(" -> ");
    print_address(beneficiary);
    printf("\n");
}

static void call(struct evm_result* result,
                 struct evm_env* env,
                 const struct evm_message* msg)
{
    printf("EVM-C: CALL (depth: %d)\n", msg->depth);
    result->code = EVM_FAILURE;
}

static void get_tx_context(struct evm_tx_context* result, struct evm_env* env)
{

}

static void get_block_hash(struct evm_uint256be* result, struct evm_env* env,
                           int64_t number)
{

}

/// EVM log callback.
///
/// @note The `evm_log` name is used to avoid conflict with `log()` C function.
static void evm_log(struct evm_env* env, const struct evm_uint160be* address,
                    const uint8_t* data, size_t data_size,
                    const struct evm_uint256be topics[], size_t topics_count)
{
    printf("EVM-C: LOG%d\n", (int)topics_count);
}

/// Example how the API is supposed to be used.
int main(int argc, char *argv[]) {
    struct evm_factory factory = examplevm_get_factory();
    if (factory.abi_version != EVM_ABI_VERSION)
        return 1;  // Incompatible ABI version.

    struct evm_instance* jit = factory.create(query,
                                              get_storage,
                                              set_storage,
                                              selfdestruct,
                                              call,
                                              get_tx_context, get_block_hash,
                                              evm_log);

    uint8_t const code[] = "Place some EVM bytecode here";
    const size_t code_size = sizeof(code);
    struct evm_uint256be code_hash = {.bytes = {1, 2, 3,}};
    uint8_t const input[] = "Hello World!";
    struct evm_uint256be value = {{1, 0,}};
    struct evm_uint160be addr = {{0, 1, 2,}};
    int64_t gas = 200000;

    struct evm_message msg = {addr, addr, value, input, sizeof(input),
                              code_hash, gas, 0};

    struct evm_result result =
        jit->execute(jit, NULL, EVM_HOMESTEAD, &msg, code, code_size);

    printf("Execution result:\n");
    if (result.code != EVM_SUCCESS) {
      printf("  EVM execution failure: %d\n", result.code);
    } else {
        printf("  Gas used: %ld\n", gas - result.gas_left);
        printf("  Gas left: %ld\n", result.gas_left);
        printf("  Output size: %zd\n", result.output_size);

        printf("  Output: ");
        size_t i = 0;
        for (i = 0; i < result.output_size; i++) {
            printf("%02x ", result.output_data[i]);
        }
        printf("\n");
    }

    if (result.release)
        result.release(&result);
    jit->destroy(jit);
}
