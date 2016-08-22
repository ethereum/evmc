#include "evm.h"

struct evm_uint256 balance(struct evm_env*, struct evm_hash160 address);

union evm_variant query(struct evm_env* env,
                        enum evm_query_key key,
                        union evm_variant arg) {
    union evm_variant result;
    switch (key) {
    case EVM_GAS_LIMIT: result.int64 = 314; break;

    case EVM_BALANCE:
        result.uint256 = balance(env, arg.address);
        break;

    default: result.int64 = 0; break;
    }
    return result;
}

/// Example how the API is supposed to be used.
void example() {
    struct evm_instance* jit = evm_create(query, 0, 0);

    char const code[] = "exec()";
    struct evm_hash256 code_hash = {.words = {1, 2, 3}};
    char const input[] = "Hello World!";
    struct evm_uint256 value = {{1, 0, 0, 0}};

    int64_t gas = 200000;
    struct evm_result result =
        evm_execute(jit, NULL, EVM_FRONTIER, code_hash, (uint8_t const*)code,
                    sizeof(code), gas, (uint8_t const*)input,
                    sizeof(input), value);

    evm_release_result_resources(&result);
    evm_destroy(jit);
}
