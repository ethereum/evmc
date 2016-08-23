#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "evm.h"

struct evm_instance {
    evm_query_fn query_fn;
    evm_update_fn update_fn;
    evm_call_fn call_fn;
};

EXPORT char const* evm_get_info(enum evm_info_key key)
{
  switch(key) {
    case EVM_NAME: return "ExampleVM"; break;
    case EVM_VERSION: return "git"; break;
  }

  return "";
}

EXPORT struct evm_instance* evm_create(evm_query_fn query_fn,
                                       evm_update_fn update_fn,
                                       evm_call_fn call_fn)
{
    struct evm_instance *ret = calloc(1, sizeof(struct evm_instance));

    if (ret) {
      ret->query_fn = query_fn;
      ret->update_fn = update_fn;
      ret->call_fn = call_fn;
    }

    return ret;
}

EXPORT void evm_destroy(struct evm_instance* evm)
{
    free(evm);
}

EXPORT int evm_set_option(struct evm_instance* evm,
                          char const* name,
                          char const* value)
{
    return 0;
}

EXPORT struct evm_result evm_execute(struct evm_instance* instance,
                                     struct evm_env* env,
                                     enum evm_mode mode,
                                     struct evm_hash256 code_hash,
                                     uint8_t const* code,
                                     size_t code_size,
                                     int64_t gas,
                                     uint8_t const* input,
                                     size_t input_size,
                                     struct evm_uint256 value)
{
    struct evm_result ret;

    memset(&ret, 0, sizeof(struct evm_result));

    // Execute code and refer to callbacks: instance->query_fn()

    ret.gas_left = 0;

    return ret;
}

EXPORT void evm_destroy_result(struct evm_result result)
{
}

EXPORT bool evmjit_is_code_ready(struct evm_instance* instance,
                                 enum evm_mode mode,
                                 struct evm_hash256 code_hash)
{
    return true;
}

EXPORT void evmjit_compile(struct evm_instance* instance,
                           enum evm_mode mode,
                           uint8_t const* code,
                           size_t code_size,
                           struct evm_hash256 code_hash)
{
}
