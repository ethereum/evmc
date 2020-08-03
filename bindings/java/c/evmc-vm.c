/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019-2020 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#include "host.h"
#include "org_ethereum_evmc_EvmcVm.h"
#include <evmc/helpers.h>
#include <evmc/loader.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

JNIEXPORT jobject JNICALL Java_org_ethereum_evmc_EvmcVm_load_1and_1create(JNIEnv* jenv,
                                                                          jclass jcls,
                                                                          jstring jfilename)
{
    (void)jcls;
    struct evmc_vm* evm = NULL;
    jint rs = evmc_java_set_jvm(jenv);
    (void)rs;
    assert(rs == JNI_OK);
    // load the EVM
    const char* filename = (*jenv)->GetStringUTFChars(jenv, jfilename, NULL);
    assert(filename != NULL);
    enum evmc_loader_error_code loader_error;
    evm = evmc_load_and_create(filename, &loader_error);
    (*jenv)->ReleaseStringUTFChars(jenv, jfilename, filename);
    if (loader_error != EVMC_LOADER_SUCCESS)
    {
        const char* error_msg = evmc_last_error_msg();
        jclass exception_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/EvmcLoaderException");
        assert(exception_class != NULL);
        (*jenv)->ThrowNew(jenv, exception_class, error_msg ? error_msg : "Loading EVMC VM failed");
    }
    jobject jresult = (*jenv)->NewDirectByteBuffer(jenv, (void*)evm, sizeof(struct evmc_vm));
    assert(jresult != NULL);
    return jresult;
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_abi_1version(JNIEnv* jenv, jclass jcls)
{
    (void)jenv;
    (void)jcls;
    return EVMC_ABI_VERSION;
}

JNIEXPORT jstring JNICALL Java_org_ethereum_evmc_EvmcVm_name(JNIEnv* jenv,
                                                             jclass jcls,
                                                             jobject jevm)
{
    (void)jcls;
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    const char* evm_name = evmc_vm_name(evm);
    return (*jenv)->NewStringUTF(jenv, evm_name);
}

JNIEXPORT jstring JNICALL Java_org_ethereum_evmc_EvmcVm_version(JNIEnv* jenv,
                                                                jclass jcls,
                                                                jobject jevm)
{
    (void)jcls;
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    const char* evm_version = evmc_vm_version(evm);
    return (*jenv)->NewStringUTF(jenv, evm_version);
}

JNIEXPORT void JNICALL Java_org_ethereum_evmc_EvmcVm_destroy(JNIEnv* jenv,
                                                             jclass jcls,
                                                             jobject jevm)
{
    (void)jcls;
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    evmc_destroy(evm);
}

JNIEXPORT void JNICALL Java_org_ethereum_evmc_EvmcVm_execute(JNIEnv* jenv,
                                                             jclass jcls,
                                                             jobject jevm,
                                                             jobject jcontext,
                                                             jint jrev,
                                                             jobject jmsg,
                                                             jobject jcode,
                                                             jobject jresult)
{
    (void)jcls;
    struct evmc_message* msg = (struct evmc_message*)(*jenv)->GetDirectBufferAddress(jenv, jmsg);
    assert(msg != NULL);
    size_t code_size;
    const uint8_t* code = GetDirectBuffer(jenv, jcode, &code_size);
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    const struct evmc_host_interface* host = evmc_java_get_host_interface();
    struct evmc_result* result =
        (struct evmc_result*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result != NULL);
    *result = evmc_execute(evm, host, (struct evmc_host_context*)jcontext, (enum evmc_revision)jrev,
                           msg, code, code_size);
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_get_1capabilities(JNIEnv* jenv,
                                                                       jclass jcls,
                                                                       jobject jevm)
{
    (void)jcls;
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    return (jint)evm->get_capabilities(evm);
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_set_1option(JNIEnv* jenv,
                                                                 jclass jcls,
                                                                 jobject jevm,
                                                                 jstring jname,
                                                                 jstring jval)
{
    (void)jcls;
    struct evmc_vm* evm = (struct evmc_vm*)(*jenv)->GetDirectBufferAddress(jenv, jevm);
    assert(evm != NULL);
    const char* name = (*jenv)->GetStringUTFChars(jenv, jname, 0);
    const char* value = (*jenv)->GetStringUTFChars(jenv, jval, 0);
    assert(name != NULL);
    assert(value != NULL);
    enum evmc_set_option_result option_result = evmc_set_option(evm, name, value);
    (*jenv)->ReleaseStringUTFChars(jenv, jname, name);
    (*jenv)->ReleaseStringUTFChars(jenv, jval, value);
    return (jint)option_result;
}

JNIEXPORT jlong JNICALL Java_org_ethereum_evmc_EvmcVm_address(JNIEnv* jenv,
                                                              jclass jcls,
                                                              jobject buf)
{
    (void)jcls;
    void* p = (*jenv)->GetDirectBufferAddress(jenv, buf);
    return (jlong)p;
}

JNIEXPORT jint JNICALL Java_org_ethereum_evmc_EvmcVm_get_1result_1size(JNIEnv* jenv, jclass jcls)
{
    (void)jenv;
    (void)jcls;
    return sizeof(struct evmc_result);
}
