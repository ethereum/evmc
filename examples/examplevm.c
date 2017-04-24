#include <stdlib.h>
#include <string.h>
#include "evm.h"


struct examplevm
{
    struct evm_instance instance;
    evm_query_fn query_fn;
    evm_update_fn update_fn;
    evm_call_fn call_fn;
};

static void evm_destroy(struct evm_instance* evm)
{
    free(evm);
}

/// Example options.
///
/// VMs are allowed to omit this function implementation.
int evm_set_option(struct evm_instance* evm,
                   char const* name,
                   char const* value)
{
    if (strcmp(name, "example-option") == 0)
        return 1;
    return 0;
}

static void evm_release_result(struct evm_result const* result)
{
}

static void free_result_output_data(struct evm_result const* result)
{
    free((uint8_t*)result->output_data);
}

static struct evm_result evm_execute(struct evm_instance* instance,
                                     struct evm_env* env,
                                     enum evm_mode mode,
                                     struct evm_uint256be code_hash,
                                     uint8_t const* code,
                                     size_t code_size,
                                     int64_t gas,
                                     uint8_t const* input,
                                     size_t input_size,
                                     struct evm_uint256be value)
{
    struct evm_result ret = {};
    if (code_size == 0) {
        // In case of empty code return a fancy error message.
        const char* msg = mode == EVM_METROPOLIS ?
                          "Welcome to Metropolis!" : "Hello Ethereum!";
        ret.output_data = (const uint8_t*)msg;
        ret.output_size = strlen(msg);
        ret.code = EVM_FAILURE;
        ret.release = NULL;  // We don't need to release the constant messages.
        return ret;
    }

    struct examplevm* vm = (struct examplevm*)instance;

    // Simulate executing the answering some questions from the code.

    const char my_address_question[] = "What is my address?";
    if (code_size == strlen(my_address_question) &&
        strncmp((const char*)code, my_address_question, code_size)) {
        union evm_variant query_result;
        vm->query_fn(&query_result, env, EVM_ADDRESS, NULL);
        static const size_t address_size = sizeof(query_result.address);
        uint8_t* output_data = (uint8_t*)malloc(address_size);
        memcpy(output_data, &query_result.address, address_size);
        ret.code = EVM_SUCCESS;
        ret.output_data = output_data;
        ret.output_size = address_size;
        ret.release = &free_result_output_data;
        ret.internal_memory = NULL; // We don't need another pointer.
        return ret;
    }

    ret.release = evm_release_result;
    ret.code = EVM_FAILURE;
    ret.gas_left = 0;

    return ret;
}

static struct evm_instance* evm_create(evm_query_fn query_fn,
                                       evm_update_fn update_fn,
                                       evm_call_fn call_fn)
{
    struct examplevm* vm = calloc(1, sizeof(struct examplevm));
    struct evm_instance* interface = &vm->instance;
    interface->destroy = evm_destroy;
    interface->execute = evm_execute;
    interface->set_option = evm_set_option;
    vm->query_fn = query_fn;
    vm->update_fn = update_fn;
    vm->call_fn = call_fn;
    return interface;
}

struct evm_factory examplevm_get_factory()
{
    struct evm_factory factory = {EVM_ABI_VERSION, evm_create};
    return factory;
}
