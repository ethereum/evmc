#include <assert.h>
#include <stdlib.h>

#include "host.h"

static JavaVM* jvm;

int set_jvm(JNIEnv* jenv)
{
    return (*jenv)->GetJavaVM(jenv, &jvm);
}

static JNIEnv* attach()
{
    JNIEnv* jenv;
    jint rs = (*jvm)->AttachCurrentThread(jvm, (void**)&jenv, NULL);
    assert(rs == JNI_OK);
    return jenv;
}

static bool account_exists_fn(struct evmc_host_context* context, const evmc_address* address)
{
    bool result = false;
    const char java_method_name[] = "account_exists";
    const char java_method_signature[] = "(I[B)I";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        // call java method
        jint jresult =
            (*jenv)->CallStaticIntMethod(jenv, host_class, method, jcontext_index, jaddress);
        result = !!jresult;
    }
    return result;
}

static evmc_bytes32 get_storage_fn(struct evmc_host_context* context,
                                   const evmc_address* address,
                                   const evmc_bytes32* key)
{
    evmc_bytes32 result;
    const char java_method_name[] = "get_storage";
    const char java_method_signature[] = "(I[B[B)Ljava/nio/ByteBuffer;";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;
        jbyteArray jkey;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        jkey = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_bytes32));
        assert(jkey != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jkey, 0, sizeof(struct evmc_bytes32), (jbyte*)key);

        // call java method
        jobject jresult = (*jenv)->CallStaticObjectMethod(jenv, host_class, method, jcontext_index,
                                                          jaddress, jkey);

        assert(jresult != NULL);
        evmc_bytes32* result_ptr =
            (struct evmc_bytes32*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
        assert(result_ptr != NULL);
        result = *result_ptr;
    }
    return result;
}

static enum evmc_storage_status set_storage_fn(struct evmc_host_context* context,
                                               const evmc_address* address,
                                               const evmc_bytes32* key,
                                               const evmc_bytes32* value)
{
    enum evmc_storage_status result;
    const char java_method_name[] = "set_storage";
    const char java_method_signature[] = "(I[B[B[B)I";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;
        jbyteArray jkey;
        jbyteArray jvalue;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        jkey = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_bytes32));
        assert(jkey != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jkey, 0, sizeof(struct evmc_bytes32), (jbyte*)key);

        jvalue = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_bytes32));
        assert(jvalue != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jvalue, 0, sizeof(struct evmc_bytes32), (jbyte*)value);

        // call java method
        jint jresult = (*jenv)->CallStaticIntMethod(jenv, host_class, method, jcontext_index,
                                                    jaddress, jkey, jvalue);
        result = (enum evmc_storage_status)jresult;
    }
    return result;
}

static evmc_uint256be get_balance_fn(struct evmc_host_context* context, const evmc_address* address)
{
    evmc_uint256be result;
    char java_method_name[] = "get_balance";
    char java_method_signature[] = "(I[B)Ljava/nio/ByteBuffer;";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        // call java method
        jobject jresult =
            (*jenv)->CallStaticObjectMethod(jenv, host_class, method, jcontext_index, jaddress);
        assert(jresult != NULL);

        evmc_uint256be* result_ptr =
            (evmc_uint256be*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
        assert(result_ptr != NULL);
        result = *result_ptr;

        (*jenv)->ReleaseByteArrayElements(jenv, jaddress, (jbyte*)address, 0);
    }
    return result;
}

static size_t get_code_size_fn(struct evmc_host_context* context, const evmc_address* address)
{
    size_t result = 0;
    char java_method_name[] = "get_code_size";
    char java_method_signature[] = "(I[B)I";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        // call java method
        jint jresult =
            (*jenv)->CallStaticIntMethod(jenv, host_class, method, jcontext_index, jaddress);
        result = jresult;
    }
    return result;
}

static evmc_bytes32 get_code_hash_fn(struct evmc_host_context* context, const evmc_address* address)
{
    evmc_bytes32 result;
    char java_method_name[] = "get_code_hash";
    char java_method_signature[] = "(I[B)Ljava/nio/ByteBuffer;";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        // call java method
        jobject jresult =
            (*jenv)->CallStaticObjectMethod(jenv, host_class, method, jcontext_index, jaddress);
        assert(jresult != NULL);

        evmc_bytes32* result_ptr =
            (struct evmc_bytes32*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
        assert(result_ptr != NULL);
        result = *result_ptr;

        (*jenv)->ReleaseByteArrayElements(jenv, jaddress, (jbyte*)address, 0);
    }
    return result;
}

static size_t copy_code_fn(struct evmc_host_context* context,
                           const evmc_address* address,
                           size_t code_offset,
                           uint8_t* buffer_data,
                           size_t buffer_size)
{
    size_t result;
    const char java_method_name[] = "copy_code";
    const char java_method_signature[] = "(I[BI)Ljava/nio/ByteBuffer;";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;
        jint jcode_offset;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        jcode_offset = code_offset;

        // call java method
        jobject jresult = (*jenv)->CallStaticObjectMethod(jenv, host_class, method, jcontext_index,
                                                          jaddress, jcode_offset);
        assert(jresult != NULL);

        // copy jresult back to buffer_data
        buffer_data = (uint8_t*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
        assert(buffer_data != NULL);

        result = get_code_size_fn(context, address) - code_offset;

        (*jenv)->ReleaseByteArrayElements(jenv, jaddress, (jbyte*)address, 0);
    }
    return result;
}

static void selfdestruct_fn(struct evmc_host_context* context,
                            const evmc_address* address,
                            const evmc_address* beneficiary)
{
    const char java_method_name[] = "selfdestruct";
    const char java_method_signature[] = "(I[B[B)V";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;
        jbyteArray jbeneficiary;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        jbeneficiary = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jbeneficiary != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jbeneficiary, 0, sizeof(struct evmc_address),
                                    (jbyte*)beneficiary);

        // call java method
        (*jenv)->CallStaticIntMethod(jenv, host_class, method, jcontext_index, jaddress,
                                     jbeneficiary);
    }
    return;
}

static struct evmc_result call_fn(struct evmc_host_context* context, const struct evmc_message* msg)
{
    struct evmc_result result;
    const char java_method_name[] = "call";
    const char java_method_signature[] = "(ILjava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;";
    JNIEnv* jenv = attach();
    assert(context != NULL);
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jobject jmsg;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jmsg = (*jenv)->NewDirectByteBuffer(jenv, (void*)msg, sizeof(struct evmc_message));
        assert(jmsg != NULL);

        // call java method
        jobject jresult =
            (*jenv)->CallStaticObjectMethod(jenv, host_class, method, jcontext_index, jmsg);
        assert(jresult != NULL);

        struct evmc_result* result_ptr =
            (struct evmc_result*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
        assert(result_ptr != NULL);
        result = *result_ptr;
    }
    return result;
}

static struct evmc_tx_context get_tx_context_fn(struct evmc_host_context* context)
{
    struct evmc_tx_context result;
    const char java_method_name[] = "get_tx_context";
    const char java_method_signature[] = "(I)Ljava/nio/ByteBuffer;";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        // call java method
        jobject jresult = (*jenv)->CallStaticObjectMethod(jenv, host_class, method, jcontext_index);
        assert(jresult != NULL);

        struct evmc_tx_context* result_ptr =
            (struct evmc_tx_context*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
        assert(result_ptr != NULL);
        result = *result_ptr;
    }
    return result;
}


static evmc_bytes32 get_block_hash_fn(struct evmc_host_context* context, int64_t number)
{
    evmc_bytes32 result;
    char java_method_name[] = "get_code_hash";
    char java_method_signature[] = "(IJ)Ljava/nio/ByteBuffer;";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jlong jnumber;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jnumber = (jlong)number;

        // call java method
        jobject jresult =
            (*jenv)->CallStaticObjectMethod(jenv, host_class, method, jcontext_index, jnumber);
        assert(jresult != NULL);

        evmc_bytes32* result_ptr =
            (struct evmc_bytes32*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
        assert(result_ptr != NULL);
        result = *result_ptr;
    }
    return result;
}

static void emit_log_fn(struct evmc_host_context* context,
                        const evmc_address* address,
                        const uint8_t* data,
                        size_t data_size,
                        const evmc_bytes32 topics[],
                        size_t topics_count)
{
    const char java_method_name[] = "emit_log";
    const char java_method_signature[] = "(I[B[BI[[BI)V";
    assert(context != NULL);
    JNIEnv* jenv = attach();
    if (jenv != NULL)
    {
        jclass host_class;
        jmethodID method;
        jint jcontext_index;
        jbyteArray jaddress;
        jbyteArray jdata;
        jobjectArray jtopics;

        // get java class
        host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
        assert(host_class != NULL);

        // get java method
        method =
            (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
        assert(method != NULL);

        // set java method params
        jcontext_index = context->index;

        jaddress = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_address));
        assert(jaddress != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jaddress, 0, sizeof(struct evmc_address),
                                    (jbyte*)address);

        jdata = (*jenv)->NewByteArray(jenv, data_size);
        assert(jdata != NULL);
        (*jenv)->SetByteArrayRegion(jenv, jdata, 0, data_size, (jbyte*)data);

        jclass byte_type = (*jenv)->FindClass(jenv, "[B");
        jtopics = (*jenv)->NewObjectArray(jenv, (jsize)topics_count, byte_type, NULL);
        assert(jtopics != NULL);
        for (int i = 0; i < topics_count; i++)
        {
            jbyteArray jtopic = (*jenv)->NewByteArray(jenv, sizeof(struct evmc_bytes32));
            assert(jtopic != NULL);
            (*jenv)->SetByteArrayRegion(jenv, jtopic, 0, sizeof(struct evmc_bytes32),
                                        (jbyte*)topics[i].bytes);
            (*jenv)->SetObjectArrayElement(jenv, jtopics, i, jtopic);
            (*jenv)->DeleteLocalRef(jenv, jtopic);
        }

        // call java method
        (*jenv)->CallStaticIntMethod(jenv, host_class, method, jcontext_index, jaddress, jdata,
                                     data_size, jtopics, topics_count);
    }
    return;
}

const struct evmc_host_interface* get_host_interface()
{
    static const struct evmc_host_interface host = {
        account_exists_fn, get_storage_fn,    set_storage_fn,    get_balance_fn,
        get_code_size_fn,  get_code_hash_fn,  copy_code_fn,      selfdestruct_fn,
        call_fn,           get_tx_context_fn, get_block_hash_fn, emit_log_fn,
    };
    return &host;
}
