/// EVM-C -- C interface to Ethereum Virtual Machine
///
/// ## High level design rules
///
/// 1. Pass function arguments and results by value.
///    This rule comes from modern C++ and tries to avoid costly alias analysis
///    needed for optimization. As the result we have a lots of complex structs
///    and unions. And variable sized arrays of bytes cannot be passed by copy.
/// 2. The EVM operates on integers so it prefers values to be host-endian.
///    On the other hand, LLVM can generate good code for byte swaping.
///    The interface also tries to match host application "natural" endianess.
///    I would like to know what endianess you use and where.
///
/// ## Terms
///
/// 1. EVM  -- an Ethereum Virtual Machine instance/implementation.
/// 2. Host -- an entity controlling the EVM. The Host requests code execution
///            and responses to EVM queries by callback functions.
///
/// @defgroup EVMC EVM-C
/// @{
#ifndef EVM_H
#define EVM_H

#include <stdint.h>    // Definition of int64_t, uint64_t.
#include <stddef.h>    // Definition of size_t.

#if __cplusplus
extern "C" {
#endif

// BEGIN Python CFFI declarations

enum {
    /// The EVM-C ABI version number of the interface declared in this file.
    EVM_ABI_VERSION = 0
};

/// Big-endian 256-bit integer.
///
/// 32 bytes of data representing big-endian 256-bit integer. I.e. bytes[0] is
/// the most significant byte, bytes[31] is the least significant byte.
/// This type is used to transfer to/from the VM values interpreted by the user
/// as both 256-bit integers and 256-bit hashes.
struct evm_uint256be {
    /// The 32 bytes of the big-endian integer or hash.
    uint8_t bytes[32];
};

/// Big-endian 160-bit hash suitable for keeping an Ethereum address.
struct evm_address {
    /// The 20 bytes of the hash.
    uint8_t bytes[20];
};

/// The kind of call-like instruction.
enum evm_call_kind {
    EVM_CALL = 0,         ///< Request CALL.
    EVM_DELEGATECALL = 1, ///< Request DELEGATECALL. The value param ignored.
    EVM_CALLCODE = 2,     ///< Request CALLCODE.
    EVM_CREATE = 3,       ///< Request CREATE. Semantic of some params changes.
};

enum evm_flags {
    EVM_STATIC = 1
};

struct evm_message {
    struct evm_address address;
    struct evm_address sender;
    struct evm_uint256be value;
    const uint8_t* input;
    size_t input_size;
    struct evm_uint256be code_hash;
    int64_t gas;
    int32_t depth;
    enum evm_call_kind kind;
    uint32_t flags;
};

struct evm_tx_context {
    struct evm_uint256be tx_gas_price;
    struct evm_address tx_origin;
    struct evm_address block_coinbase;
    int64_t block_number;
    int64_t block_timestamp;
    int64_t block_gas_limit;
    struct evm_uint256be block_difficulty;
};

struct evm_context;

typedef void (*evm_get_tx_context_fn)(struct evm_tx_context* result,
                                      struct evm_context* context);

typedef void (*evm_get_block_hash_fn)(struct evm_uint256be* result,
                                      struct evm_context* context,
                                      int64_t number);

/// The execution status code.
enum evm_status_code {
    EVM_SUCCESS = 0,               ///< Execution finished with success.
    EVM_FAILURE = 1,               ///< Generic execution failure.
    EVM_OUT_OF_GAS = 2,
    EVM_BAD_INSTRUCTION = 3,
    EVM_BAD_JUMP_DESTINATION = 4,
    EVM_STACK_OVERFLOW = 5,
    EVM_STACK_UNDERFLOW = 6,
    EVM_REVERT = 7,                ///< Execution terminated with REVERT opcode.

    /// EVM implementation internal error.
    ///
    /// @todo We should rethink reporting internal errors. One of the options
    ///       it to allow using any negative value to represent internal errors.
    EVM_INTERNAL_ERROR = -1,
};

struct evm_result;  ///< Forward declaration.

/// Releases resources assigned to an execution result.
///
/// This function releases memory (and other resources, if any) assigned to the
/// specified execution result making the result object invalid.
///
/// @param result  The execution result which resource are to be released. The
///                result itself it not modified by this function, but becomes
///                invalid and user should discard it as well.
typedef void (*evm_release_result_fn)(const struct evm_result* result);

/// The EVM code execution result.
struct evm_result {
    /// The execution status code.
    enum evm_status_code status_code;

    /// The amount of gas left after the execution.
    ///
    /// If evm_result::code is not ::EVM_SUCCESS nor ::EVM_REVERT
    /// the value MUST be 0.
    int64_t gas_left;

    /// The reference to output data.
    ///
    /// The output contains data coming from RETURN opcode (iff evm_result::code
    /// field is ::EVM_SUCCESS) or from REVERT opcode.
    ///
    /// The memory containing the output data is owned by EVM and has to be
    /// freed with evm_result::release().
    uint8_t const* output_data;

    /// The size of the output data.
    size_t output_size;

    /// The pointer to a function releasing all resources associated with
    /// the result object.
    ///
    /// This function pointer is optional (MAY be NULL) and MAY be set by
    /// the EVM implementation. If set it MUST be used by the user to
    /// release memory and other resources associated with the result object.
    /// After the result resources are released the result object
    /// MUST NOT be used any more.
    ///
    /// The suggested code pattern for releasing EVM results:
    /// @code
    /// struct evm_result result = ...;
    /// if (result.release)
    ///     result.release(&result);
    /// @endcode
    ///
    /// @note
    /// It works similarly to C++ virtual destructor. Attaching the release
    /// function to the result itself allows EVM composition.
    evm_release_result_fn release;

    /// The address of the contract created by CREATE opcode.
    ///
    /// This field has valid value only if the result describes successful
    /// CREATE (evm_result::status_code is ::EVM_SUCCESS).
    struct evm_address create_address;

    /// Reserved data that MAY be used by a evm_result object creator.
    ///
    /// This reserved 4 bytes together with 20 bytes from create_address form
    /// 24 bytes of memory called "optional data" within evm_result struct
    /// to be optionally used by the evm_result object creator.
    ///
    /// @see evm_result_optional_data, evm_get_optional_data().
    ///
    /// Also extends the size of the evm_result to 64 bytes (full cache line).
    uint8_t padding[4];
};


/// The union representing evm_result "optional data".
///
/// The evm_result struct contains 24 bytes of optional data that can be
/// reused by the obejct creator if the object does not contain
/// evm_result::create_address.
///
/// An EVM implementation MAY use this memory to keep additional data
/// when returning result from ::evm_execute_fn.
/// The host application MAY use this memory to keep additional data
/// when returning result of performed calls from ::evm_call_fn.
///
/// @see evm_get_optional_data(), evm_get_const_optional_data().
union evm_result_optional_data
{
    uint8_t bytes[24];
    void* pointer;
};

/// Provides read-write access to evm_result "optional data".
static inline union evm_result_optional_data* evm_get_optional_data(
    struct evm_result* result)
{
    return (union evm_result_optional_data*) &result->create_address;
}

/// Provides read-only access to evm_result "optional data".
static inline const union evm_result_optional_data* evm_get_const_optional_data(
    const struct evm_result* result)
{
    return (const union evm_result_optional_data*) &result->create_address;
}


/// Check account existence callback function
///
/// This callback function is used by the EVM to check if
/// there exists an account at given address.
/// @param      context  The pointer to the Host execution context.
///                      @see ::evm_context.
/// @param      address  The address of the account the query is about.
/// @return              1 if exists, 0 otherwise.
typedef int (*evm_account_exists_fn)(struct evm_context* context,
                                     const struct evm_address* address);

/// Get storage callback function.
///
/// This callback function is used by an EVM to query the given contract
/// storage entry.
/// @param[out] result   The returned storage value.
/// @param      context  The pointer to the Host execution context.
///                      @see ::evm_context.
/// @param      address  The address of the contract.
/// @param      key      The index of the storage entry.
typedef void (*evm_get_storage_fn)(struct evm_uint256be* result,
                                   struct evm_context* context,
                                   const struct evm_address* address,
                                   const struct evm_uint256be* key);

/// Set storage callback function.
///
/// This callback function is used by an EVM to update the given contract
/// storage entry.
/// @param context  The pointer to the Host execution context.
///                 @see ::evm_context.
/// @param address  The address of the contract.
/// @param key      The index of the storage entry.
/// @param value    The value to be stored.
typedef void (*evm_set_storage_fn)(struct evm_context* context,
                                   const struct evm_address* address,
                                   const struct evm_uint256be* key,
                                   const struct evm_uint256be* value);

/// Get balance callback function.
///
/// This callback function is used by an EVM to query the balance of the given
/// address.
/// @param[out] result   The returned balance value.
/// @param      context  The pointer to the Host execution context.
///                      @see ::evm_context.
/// @param      address  The address.
typedef void (*evm_get_balance_fn)(struct evm_uint256be* result,
                                   struct evm_context* context,
                                   const struct evm_address* address);

/// Get code callback function.
///
/// This callback function is used by an EVM to get the code of a contract of
/// given address.
/// @param[out] result_code  The pointer to the contract code. This argument is
///                          optional. If NULL is provided, the host MUST only
///                          return the code size.
/// @param      context      The pointer to the Host execution context.
///                          @see ::evm_context.
/// @param      address      The address of the contract.
/// @return                  The size of the code.
typedef size_t (*evm_get_code_fn)(const uint8_t** result_code,
                                  struct evm_context* context,
                                  const struct evm_address* address);

/// Selfdestruct callback function.
///
/// This callback function is used by an EVM to SELFDESTRUCT given contract.
/// @param context      The pointer to the Host execution context.
///                     @see ::evm_context.
/// @param address      The address of the contract to be selfdestructed.
/// @param beneficiary  The address where the remaining ETH is going to be
///                     transferred.
typedef void (*evm_selfdestruct_fn)(struct evm_context* context,
                                    const struct evm_address* address,
                                    const struct evm_address* beneficiary);

/// Log callback function.
///
/// This callback function is used by an EVM to inform about a LOG that happened
/// during an EVM bytecode execution.
/// @param context       The pointer to the Host execution context.
///                      @see ::evm_context.
/// @param address       The address of the contract that generated the log.
/// @param data          The pointer to unindexed data attached to the log.
/// @param data_size     The length of the data.
/// @param topics        The pointer to the array of topics attached to the log.
/// @param topics_count  The number of the topics. Valid values are between
///                      0 and 4 inclusively.
typedef void (*evm_log_fn)(struct evm_context* context,
                           const struct evm_address* address,
                           const uint8_t* data,
                           size_t data_size,
                           const struct evm_uint256be topics[],
                           size_t topics_count);

/// Pointer to the callback function supporting EVM calls.
///
/// @param[out] result  The result of the call. The result object is not
///                     initialized by the EVM, the Client MUST correctly
///                     initialize all expected fields of the structure.
/// @param      context The pointer to the Host execution context.
///                     @see ::evm_context.
/// @param      msg     Call parameters.
typedef void (*evm_call_fn)(struct evm_result* result,
                            struct evm_context* context,
                            const struct evm_message* msg);

/// The context interface.
///
/// The set of all callback functions expected by EVM instances. This is C
/// realisation of vtable for OOP interface (only virtual methods, no data).
/// Host implementations SHOULD create constant singletons of this (similarly
/// to vtables) to lower the maintenance and memory management cost.
struct evm_context_fn_table {
    evm_account_exists_fn account_exists;
    evm_get_storage_fn get_storage;
    evm_set_storage_fn set_storage;
    evm_get_balance_fn get_balance;
    evm_get_code_fn get_code;
    evm_selfdestruct_fn selfdestruct;
    evm_call_fn call;
    evm_get_tx_context_fn get_tx_context;
    evm_get_block_hash_fn get_block_hash;
    evm_log_fn log;
};


/// Execution context managed by the Host.
///
/// The Host MUST pass the pointer to the execution context to
/// ::evm_execute_fn. The EVM MUST pass the same pointer back to the Host in
/// every callback function.
/// The context MUST contain at least the function table defining the context
/// callback interface.
/// Optionally, The Host MAY include in the context additional data.
struct evm_context {

    /// Function table defining the context interface (vtable).
    const struct evm_context_fn_table* fn_table;
};


struct evm_instance;  ///< Forward declaration.

/// Destroys the EVM instance.
///
/// @param evm  The EVM instance to be destroyed.
typedef void (*evm_destroy_fn)(struct evm_instance* evm);


/// Configures the EVM instance.
///
/// Allows modifying options of the EVM instance.
/// Options:
/// - code cache behavior: on, off, read-only, ...
/// - optimizations,
///
/// @param evm    The EVM instance to be configured.
/// @param name   The option name. NULL-terminated string. Cannot be NULL.
/// @param value  The new option value. NULL-terminated string. Cannot be NULL.
/// @return       1 if the option set successfully, 0 otherwise.
typedef int (*evm_set_option_fn)(struct evm_instance* evm,
                                 char const* name,
                                 char const* value);


/// EVM revision.
///
/// The revision of the EVM specification based on the Ethereum
/// upgrade / hard fork codenames.
enum evm_revision {
    EVM_FRONTIER = 0,
    EVM_HOMESTEAD = 1,
    EVM_TANGERINE_WHISTLE = 2,
    EVM_SPURIOUS_DRAGON = 3,
    EVM_BYZANTIUM = 4,
    EVM_CONSTANTINOPLE = 5,
};


/// Generates and executes machine code for given EVM bytecode.
///
/// All the fun is here. This function actually does something useful.
///
/// @param instance    A EVM instance.
/// @param context     The pointer to the Host execution context to be passed
///                    to callback functions. @see ::evm_context.
/// @param rev         Requested EVM specification revision.
/// @param code_hash   A hash of the bytecode, usually Keccak. The EVM uses it
///                    as the code identifier. A EVM implementation is able to
///                    hash the code itself if it requires it, but the host
///                    application usually has the hash already.
/// @param code        Reference to the bytecode to be executed.
/// @param code_size   The length of the bytecode.
/// @param gas         Gas for execution. Min 0, max 2^63-1.
/// @param input       Reference to the input data.
/// @param input_size  The size of the input data.
/// @param value       Call value.
/// @return            All execution results.
typedef struct evm_result (*evm_execute_fn)(struct evm_instance* instance,
                                            struct evm_context* context,
                                            enum evm_revision rev,
                                            const struct evm_message* msg,
                                            uint8_t const* code,
                                            size_t code_size);


/// Status of a code in VM. Useful for JIT-like implementations.
enum evm_code_status {
    /// The code is uknown to the VM.
    EVM_UNKNOWN,

    /// The code has been compiled and is available in memory.
    EVM_READY,

    /// The compiled version of the code is available in on-disk cache.
    EVM_CACHED,
};


/// Get information the status of the code in the VM.
typedef enum evm_code_status
(*evm_get_code_status_fn)(struct evm_instance* instance,
                          enum evm_revision rev,
                          uint32_t flags,
                          struct evm_uint256be code_hash);

/// Request preparation of the code for faster execution. It is not required
/// to execute the code but allows compilation of the code ahead of time in
/// JIT-like VMs.
typedef void (*evm_prepare_code_fn)(struct evm_instance* instance,
                                    enum evm_revision rev,
                                    uint32_t flags,
                                    struct evm_uint256be code_hash,
                                    uint8_t const* code,
                                    size_t code_size);

/// The EVM instance.
///
/// Defines the base struct of the EVM implementation.
struct evm_instance {

    /// EVM-C ABI version implemented by the EVM instance.
    ///
    /// For future use to detect ABI incompatibilities. The EVM-C ABI version
    /// represented by this file is in ::EVM_ABI_VERSION.
    ///
    /// @todo Consider removing this field.
    const int abi_version;

    /// Pointer to function destroying the EVM instance.
    evm_destroy_fn destroy;

    /// Pointer to function executing a code by the EVM instance.
    evm_execute_fn execute;

    /// Optional pointer to function returning a status of a code.
    ///
    /// If the VM does not support this feature the pointer can be NULL.
    evm_get_code_status_fn get_code_status;

    /// Optional pointer to function compiling  a code.
    ///
    /// If the VM does not support this feature the pointer can be NULL.
    evm_prepare_code_fn prepare_code;

    /// Optional pointer to function modifying VM's options.
    ///
    /// If the VM does not support this feature the pointer can be NULL.
    evm_set_option_fn set_option;
};

// END Python CFFI declarations

/// Example of a function creating an instance of an example EVM implementation.
///
/// Each EVM implementation MUST provide a function returning an EVM instance.
/// The function SHOULD be named `<vm-name>_create(void)`.
///
/// @return  EVM instance or NULL indicating instance creation failure.
struct evm_instance* examplevm_create(void);


#if __cplusplus
}
#endif

#endif  // EVM_H
/// @}
