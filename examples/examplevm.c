#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "evm.h"


struct examplevm
{
    struct evm_instance instance;
    evm_query_state_fn query_fn;
    evm_update_state_fn update_fn;
    evm_call_fn call_fn;
    evm_get_tx_context_fn get_tx_context_fn;
    evm_get_block_hash_fn get_block_hash_fn;

    int example_option;
};

static void evm_destroy(struct evm_instance* evm)
{
    free(evm);
}

/// Example options.
///
/// VMs are allowed to omit this function implementation.
int evm_set_option(struct evm_instance* instance,
                   char const* name,
                   char const* value)
{
    struct examplevm* vm = (struct examplevm*)instance;
    if (strcmp(name, "example-option") == 0) {
        long int v = strtol(value, NULL, 0);
        if (v > INT_MAX || v < INT_MIN)
            return 0;
        vm->example_option = (int)v;
        return 1;
    }

    return 0;
}

static void evm_release_result(struct evm_result const* result)
{
}

static void free_result_output_data(struct evm_result const* result)
{
    free((uint8_t*)result->output_data);
}

static struct evm_result execute(struct evm_instance* instance,
                                 struct evm_env* env,
                                 enum evm_mode mode,
                                 const struct evm_message* msg,
                                 const uint8_t* code,
                                 size_t code_size)
{
    struct evm_result ret = {};
    if (code_size == 0) {
        // In case of empty code return a fancy error message.
        const char* error = mode == EVM_METROPOLIS ?
                            "Welcome to Metropolis!" : "Hello Ethereum!";
        ret.output_data = (const uint8_t*)error;
        ret.output_size = strlen(error);
        ret.code = EVM_FAILURE;
        ret.release = NULL;  // We don't need to release the constant messages.
        return ret;
    }

    struct examplevm* vm = (struct examplevm*)instance;

    // Simulate executing by checking for some code patterns.
    // Solidity inline assembly is used in the examples instead of EVM bytecode.

    // Assembly: `{ mstore(0, address()) return(0, msize()) }`.
    const char return_address[] = "30600052596000f3";

    // Assembly: `{ sstore(0, add(sload(0), 1)) }`
    const char counter[] = "600160005401600055";

    if (code_size == strlen(return_address) &&
        strncmp((const char*)code, return_address, code_size)) {
        static const size_t address_size = sizeof(msg->address);
        uint8_t* output_data = (uint8_t*)malloc(address_size);
        if (!output_data) {
            // malloc failed, report internal error.
            ret.code = EVM_INTERNAL_ERROR;
            return ret;
        }
        memcpy(output_data, &msg->address, address_size);
        ret.code = EVM_SUCCESS;
        ret.output_data = output_data;
        ret.output_size = address_size;
        ret.release = &free_result_output_data;
        ret.context = NULL; // We don't need another pointer.
        return ret;
    }
    else if (code_size == strlen(counter) &&
        strncmp((const char*)code, counter, code_size)) {
        union evm_variant value;
        const struct evm_uint256be index = {0, 0, 0, 0};
        vm->query_fn(&value, env, EVM_SLOAD, &msg->address, &index);
        value.uint256be.bytes[31] += 1;
        union evm_variant arg;
        arg.uint256be = index;
        vm->update_fn(env, EVM_SSTORE, &msg->address, &arg, &value);
        ret.code = EVM_SUCCESS;
        return ret;
    }

    ret.release = evm_release_result;
    ret.code = EVM_FAILURE;
    ret.gas_left = 0;

    return ret;
}

static struct evm_instance* evm_create(evm_query_state_fn query_fn,
                                       evm_update_state_fn update_fn,
                                       evm_call_fn call_fn,
                                       evm_get_tx_context_fn get_tx_context_fn,
                                       evm_get_block_hash_fn get_block_hash_fn)
{
    struct examplevm* vm = calloc(1, sizeof(struct examplevm));
    struct evm_instance* interface = &vm->instance;
    interface->destroy = evm_destroy;
    interface->execute = execute;
    interface->set_option = evm_set_option;
    vm->query_fn = query_fn;
    vm->update_fn = update_fn;
    vm->call_fn = call_fn;
    vm->get_tx_context_fn = get_tx_context_fn;
    vm->get_block_hash_fn = get_block_hash_fn;
    return interface;
}

struct evm_factory examplevm_get_factory()
{
    struct evm_factory factory = {EVM_ABI_VERSION, evm_create};
    return factory;
}
