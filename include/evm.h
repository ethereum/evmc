/// EVM-C -- C interface to Ethereum Virtual Machine
///
/// ## High level design rules
/// 1. Pass function arguments and results by value.
///    This rule comes from modern C++ and tries to avoid costly alias analysis
///    needed for optimization. As the result we have a lots of complex structs
///    and unions. And variable sized arrays of bytes cannot be passed by copy.
/// 2. The EVM operates on integers so it prefers values to be host-endian.
///    On the other hand, LLVM can generate good code for byte swaping.
///    The interface also tries to match host application "natural" endianess.
///    I would like to know what endianess you use and where.
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

/// Opaque struct representing execution environment managed by the host
/// application.
struct evm_env;

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
/// TODO: Rename to "address".
struct evm_uint160be {
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
    struct evm_uint160be address;
    struct evm_uint160be sender;
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
    struct evm_uint160be tx_origin;
    struct evm_uint160be block_coinbase;
    int64_t block_number;
    int64_t block_timestamp;
    int64_t block_gas_limit;
    struct evm_uint256be block_difficulty;
};

typedef void (*evm_get_tx_context_fn)(struct evm_tx_context* result,
                                      struct evm_env* env);

typedef void (*evm_get_block_hash_fn)(struct evm_uint256be* result,
                                      struct evm_env* env,
                                      int64_t number);

/// The execution result code.
enum evm_result_code {
    EVM_SUCCESS = 0,               ///< Execution finished with success.
    EVM_FAILURE = 1,               ///< Generic execution failure.
    EVM_OUT_OF_GAS = 2,
    EVM_BAD_INSTRUCTION = 3,
    EVM_BAD_JUMP_DESTINATION = 4,
    EVM_STACK_OVERFLOW = 5,
    EVM_STACK_UNDERFLOW = 6,
    EVM_REVERT = 7,  ///< Execution terminated with REVERT opcode.

    /// EVM implementation internal error.
    ///
    /// FIXME: We should rethink reporting internal errors. One of the options
    /// it to allow using any negative value to represent internal errors.
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
typedef void (*evm_release_result_fn)(struct evm_result const* result);

/// The EVM code execution result.
struct evm_result {
    /// The execution result code.
    /// FIXME: Rename to 'status' or 'status_code'.
    enum evm_result_code code;

    /// The amount of gas left after the execution.
    ///
    /// The value is valid only if evm_result::code is ::EVM_SUCCESS
    /// or ::EVM_REVERT. In other cases all provided gas is assumed to have been
    /// used.
    int64_t gas_left;

    /// The reference to output data.
    ///
    /// The output contains data coming from RETURN opcode (iff evm_result::code
    /// field is ::EVM_SUCCESS) or from REVERT opcode (iff evm_result::code
    /// field is ::EVM_REVERT).
    ///
    /// In case the evm_result::code field signals
    /// a failure the output MAY contain optional explanation of the failure
    /// for debugging or tracing purposes. In case the explanation is provided
    /// and contains human-readable text then UTF-8 encoding SHOULD be used.
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

    /// Reserved data that MAY be used by a evm_result object creator.
    ///
    /// This reserved 24 bytes extends the size of the evm_result to 64 bytes
    /// (full cache line).
    /// An EVM implementation MAY use this memory to keep additional data
    /// when returning result from ::evm_execute_fn.
    /// The host application MAY use this memory to keep additional data
    /// when returning result of performed calls from ::evm_call_fn.
    union
    {
        void* context;     ///< A pointer for storing external objects.
        uint8_t data[24];  ///< 24 bytes of reserved data.
    } reserved;
};

/// The query callback key.
enum evm_query_key {
    EVM_CODE_BY_ADDRESS = 10, ///< Code by an address for EXTCODECOPY.
    EVM_CODE_SIZE = 11,       ///< Code size by an address for EXTCODESIZE.
    EVM_BALANCE = 12,         ///< Balance of a given address for BALANCE.
    EVM_ACCOUNT_EXISTS = 14,  ///< Check if an account exists.
};


/// Variant type to represent possible types of values used in EVM.
///
/// Type-safety is lost around the code that uses this type. We should have
/// complete set of unit tests covering all possible cases.
/// The size of the type is 64 bytes and should fit in single cache line.
union evm_variant {
    /// A host-endian 64-bit integer.
    int64_t int64;

    /// A big-endian 256-bit integer or hash.
    struct evm_uint256be uint256be;

    /// A memory reference.
    struct {
        /// Pointer to the data.
        uint8_t const* data;

        /// Size of the referenced memory/data.
        size_t data_size;
    };
};

/// Query callback function.
///
/// This callback function is used by the EVM to query the host application
/// about additional information about accounts in the state required to
/// execute EVM code.
/// @param[out] result  The result of the query.
/// @param      env     Pointer to execution environment managed by the host
///                     application.
/// @param      key     The kind of the query. See evm_query_key
///                     and details below.
/// @param      address  The address of the account the query is about.
///
/// ## Types of queries
///
/// - ::EVM_CODE_BY_ADDRESS
///   @result evm_variant::data       The appropriate code for the given address or NULL if not found.
///
/// - ::EVM_CODE_SIZE
///   @result evm_variant::int64      The appropriate code size for the given address or 0 if not found.
///
/// - ::EVM_BALANCE
///   @result evm_variant::uint256be  The appropriate balance for the given address or 0 if not found.
///
/// - ::EVM_ACCOUNT_EXISTS
///   @result evm_variant::int64      1 if exists, 0 if not.
///
///
/// @todo
/// - Consider swapping key and address arguments,
///   e.g. `query(result, env, addr, EVM_SLOAD, k)`.
/// - Consider renaming key argument to something else. Key is confusing
///   especially for SSTORE and SLOAD. Maybe "kind"?
typedef void (*evm_query_state_fn)(union evm_variant* result,
                                   struct evm_env* env,
                                   enum evm_query_key key,
                                   const struct evm_uint160be* address);

/// Get storage callback function.
///
/// This callback function is used by an EVM to query the given contract
/// storage entry.
/// @param[out] result   The returned storage value.
/// @param      env      Pointer to execution environment managed by the host
///                      application.
/// @param      address  The address of the contract.
/// @param      key      The index of the storage entry.
typedef void (*evm_get_storage_fn)(struct evm_uint256be* result,
                                   struct evm_env* env,
                                   const struct evm_uint160be* address,
                                   const struct evm_uint256be* key);

/// Set storage callback function.
///
/// This callback function is used by an EVM to update the given contract
/// storage entry.
/// @param env      Pointer to execution environment managed by the host
///                 application.
/// @param address  The address of the contract.
/// @param key      The index of the storage entry.
/// @param value    The value to be stored.
typedef void (*evm_set_storage_fn)(struct evm_env* env,
                                   const struct evm_uint160be* address,
                                   const struct evm_uint256be* key,
                                   const struct evm_uint256be* value);

/// Selfdestruct callback function.
///
/// This callback function is used by an EVM to SELFDESTRUCT given contract.
/// @param env          The pointer to the execution environment managed by
///                     the host application.
/// @param address      The address of the contract to be selfdestructed.
/// @param beneficiary  The address where the remaining ETH is going to be
///                     transferred.
typedef void (*evm_selfdestruct_fn)(struct evm_env* env,
                                    const struct evm_uint160be* address,
                                    const struct evm_uint160be* beneficiary);

/// Log callback function.
///
/// This callback function is used by an EVM to inform about a LOG that happened
/// during an EVM bytecode execution.
/// @param env           The pointer to execution environment managed by
///                      the host application.
/// @param address       The address of the contract that generated the log.
/// @param data          The pointer to unindexed data attached to the log.
/// @param data_size     The length of the data.
/// @param topics        The pointer to the array of topics attached to the log.
/// @param topics_count  The number of the topics. Valid values are between
///                      0 and 4 inclusively.
typedef void (*evm_log_fn)(struct evm_env* env,
                           const struct evm_uint160be* address,
                           const uint8_t* data,
                           size_t data_size,
                           const struct evm_uint256be topics[],
                           size_t topics_count);

/// Pointer to the callback function supporting EVM calls.
///
/// @param[out] result  Call result.
/// @param      env     Pointer to execution environment managed by the host
///                     application.
/// @param      msg     Call parameters.
typedef void (*evm_call_fn)(
    struct evm_result* result,
    struct evm_env* env,
    const struct evm_message* msg);


struct evm_instance;  ///< Forward declaration.

/// Creates the EVM instance.
///
/// Creates and initializes an EVM instance by providing the information
/// about runtime callback functions.
///
/// @param query_fn   Pointer to query callback function. Nonnull.
/// @param update_fn  Pointer to update callback function. Nonnull.
/// @param call_fn    Pointer to call callback function. Nonnull.
/// @param get_tx_context_fn  Pointer to get tx context function. Nonnull.
/// @param get_block_hash_fn  Pointer to get block hash function. Nonnull.
/// @return           Pointer to the created EVM instance.
typedef struct evm_instance* (*evm_create_fn)(evm_query_state_fn query_fn,
                                              evm_get_storage_fn get_storage_fn,
                                              evm_set_storage_fn set_storage_fn,
                                              evm_selfdestruct_fn selfdestruct_fn,
                                              evm_call_fn call_fn,
                                              evm_get_tx_context_fn get_tx_context_fn,
                                              evm_get_block_hash_fn get_block_hash_fn,
                                              evm_log_fn log_fn);

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


/// EVM compatibility mode aka chain mode.
/// The names for the last two hard forks come from Python implementation.
enum evm_mode {
    EVM_FRONTIER = 0,
    EVM_HOMESTEAD = 1,
    EVM_ANTI_DOS = 2,
    EVM_CLEARING = 3,
    EVM_METROPOLIS = 4,
};


/// Generates and executes machine code for given EVM bytecode.
///
/// All the fun is here. This function actually does something useful.
///
/// @param instance    A EVM instance.
/// @param env         A pointer to the execution environment provided by the
///                    user and passed to callback functions.
/// @param mode        EVM compatibility mode.
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
                                            struct evm_env* env,
                                            enum evm_mode mode,
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
                          enum evm_mode mode,
                          uint32_t flags,
                          struct evm_uint256be code_hash);

/// Request preparation of the code for faster execution. It is not required
/// to execute the code but allows compilation of the code ahead of time in
/// JIT-like VMs.
typedef void (*evm_prepare_code_fn)(struct evm_instance* instance,
                                    enum evm_mode mode,
                                    uint32_t flags,
                                    struct evm_uint256be code_hash,
                                    uint8_t const* code,
                                    size_t code_size);

/// The EVM instance.
///
/// Defines the base struct of the EVM implementation.
struct evm_instance {
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

/// The EVM instance factory.
///
/// Provides ABI protection and method to create an EVM instance.
struct evm_factory {
    /// EVM-C ABI version implemented by the EVM factory and instance.
    ///
    /// For future use to detect ABI incompatibilities. The EVM-C ABI version
    /// represented by this file is in ::EVM_ABI_VERSION.
    int abi_version;

    /// Pointer to function creating and initializing the EVM instance.
    evm_create_fn create;
};

// END Python CFFI declarations

/// Example of a function creating uninitialized instance of an example VM.
///
/// Each EVM implementation is obligated to provided a function returning
/// an EVM instance.
/// The function has to be named as `<vm-name>_get_factory(void)`.
///
/// @return  EVM instance.
struct evm_factory examplevm_get_factory(void);


#if __cplusplus
}
#endif

#endif  // EVM_H
/// @}
