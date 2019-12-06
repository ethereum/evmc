#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "evmc-vm.h"
#include "evmc/helpers.h"
#include "evmc/loader.h"
#include "host.h"

JNIEXPORT jobject JNICALL Java_org_ethereum_evmc_EvmcVm_init(JNIEnv* jenv,
                                                             jclass jcls,
                                                             jstring jfilename)
{
    struct evmc_vm* evm;
    jint rs = set_jvm(jenv);
    assert(rs == JNI_OK);
    // load the EVM
    const char* filename = (*jenv)->GetStringUTFChars(jenv, jfilename, NULL);
    if (filename == NULL)
    {
        jclass jclazz = (*jenv)->FindClass(jenv, "java/lang/AssertionError");
        (*jenv)->ThrowNew(jenv, jclazz, "JNI Error: filename cannot be NULL.\n");
    }
    enum evmc_loader_error_code loader_error;
    evm = evmc_load_and_create(filename, &loader_error);
    (*jenv)->ReleaseStringUTFChars(jenv, jfilename, filename);
    if (loader_error != EVMC_LOADER_SUCCESS)
    {
        const char* error_msg = evmc_last_error_msg();
        jclass jclazz = (*jenv)->FindClass(jenv, "java/lang/AssertionError");
        (*jenv)->ThrowNew(jenv, jclazz, error_msg ? error_msg : "Loading EVMC VM failed");
    }
    jobject jresult = (*jenv)->NewDirectByteBuffer(jenv, (void*)evm, sizeof(struct evmc_vm));
    assert(jresult != NULL);
    return jresult;
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_abi_1version(JNIEnv* jenv, jclass jcls)
{
    return EVMC_ABI_VERSION;
}

JNIEXPORT jstring JNICALL Java_org_ethereum_evmc_EvmcVm_name(JNIEnv* jenv,
                                                             jclass jcls,
                                                             jobject jevm)
{
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    const char* evm_name = evmc_vm_name(evm);
    return (*jenv)->NewStringUTF(jenv, evm_name);
}

JNIEXPORT jstring JNICALL Java_org_ethereum_evmc_EvmcVm_version(JNIEnv* jenv,
                                                                jclass jcls,
                                                                jobject jevm)
{
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    const char* evm_version = evmc_vm_version(evm);
    return (*jenv)->NewStringUTF(jenv, evm_version);
}

JNIEXPORT void JNICALL Java_org_ethereum_evmc_EvmcVm_destroy(JNIEnv* jenv,
                                                             jclass jcls,
                                                             jobject jevm)
{
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    evmc_destroy(evm);
}

JNIEXPORT void JNICALL Java_org_ethereum_evmc_EvmcVm_execute(JNIEnv* jenv,
                                                             jclass jcls,
                                                             jobject jevm,
                                                             jint jcontext_index,
                                                             jint jrev,
                                                             jobject jmsg,
                                                             jobject jcode,
                                                             jint jsize,
                                                             jobject jresult)
{
    struct evmc_message* cmsg = (struct evmc_message*)(*jenv)->GetDirectBufferAddress(jenv, jmsg);
    assert(cmsg != NULL);
    const uint8_t* ccode = (uint8_t*)(*jenv)->GetDirectBufferAddress(jenv, jcode);
    assert(ccode != NULL);
    struct evmc_host_context context = {jcontext_index};
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
#ifdef DEBUG
    printf("********************before execute*******************\n");

    printf("struct: evmc_message=%p\n", cmsg);
    printf("sizeof(evmc_message): %lu\n", sizeof(struct evmc_message));
    printf("kind=%p\n", &cmsg->kind);
    printf("flags=%p\n", &cmsg->flags);
    printf("depth=%p\n", &cmsg->depth);
    printf("gas=%p\n", &cmsg->gas);
    printf("destination=%p\n", &cmsg->destination.bytes);
    printf("sender=%p\n", &cmsg->sender.bytes);
    printf("input_data=%p\n", &cmsg->input_data);
    printf("input_size=%p\n", &cmsg->input_size);
    printf("value=%p\n", &cmsg->value.bytes);
    printf("create2_salt=%p\n\n", &cmsg->create2_salt.bytes);

    printf("kind=%d\n", cmsg->kind);
    printf("flags=%d\n", cmsg->flags);
    printf("depth=%d\n", cmsg->depth);
    printf("gas=%lld\n", cmsg->gas);
    printf("destination=%s\n", cmsg->destination.bytes);
    printf("sender=%s\n", cmsg->sender.bytes);
    printf("input_size=%zu\n", cmsg->input_size);
    printf("value=%s\n\n", cmsg->value.bytes);
#endif
    const struct evmc_host_interface* host = get_host_interface();
    struct evmc_result* result =
        (struct evmc_result*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result != NULL);
    *result = evmc_execute(evm, host, &context, jrev, cmsg, ccode, jsize);
#ifdef DEBUG
    printf("********************after execute*******************\n");
    printf("sizeof(evmc_result): %lu\n", sizeof(struct evmc_result));
    printf("status_code=%p\n", &result->status_code);
    printf("gas_left=%p\n", &result->gas_left);
    printf("output_data=%p\n\n", &result->output_data);

    printf("status_code=%d\n", result->status_code);
    printf("gas_left=%llu\n", result->gas_left);
    printf("output_data=%s\n\n", result->output_data);
#endif
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_get_1capabilities(JNIEnv* jenv,
                                                                       jclass jcls,
                                                                       jobject jevm)
{
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    return evm->get_capabilities(evm);
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_set_1option(JNIEnv* jenv,
                                                                 jclass jcls,
                                                                 jobject jevm,
                                                                 jstring jname,
                                                                 jstring jvalue)
{
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    const char* name = (*jenv)->GetStringUTFChars(jenv, jname, 0);
    const char* value = (*jenv)->GetStringUTFChars(jenv, jvalue, 0);
    enum evmc_set_option_result option_result = evmc_set_option(evm, name, value);
    (*jenv)->ReleaseStringUTFChars(jenv, jname, name);
    (*jenv)->ReleaseStringUTFChars(jenv, jvalue, value);
    return option_result;
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_get_1result_1size(JNIEnv* jenv, jclass jcls)
{
    return sizeof(struct evmc_result);
}
