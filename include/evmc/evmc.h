/// EVMC -- Ethereum Client-VM Connector API
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
/// @defgroup EVMC EVMC
/// @{
#ifndef EVMC_H
#define EVMC_H

#include <stdint.h>    // Definition of int64_t, uint64_t.
#include <stddef.h>    // Definition of size_t.

#if __cplusplus
extern "C" {
#endif

// BEGIN Python CFFI declarations

enum
{
    /// The EVMC ABI version number of the interface declared in this file.
    EVMC_ABI_VERSION = 1
};

/// Big-endian 256-bit integer.
///
/// 32 bytes of data representing big-endian 256-bit integer. I.e. bytes[0] is
/// the most significant byte, bytes[31] is the least significant byte.
/// This type is used to transfer to/from the VM values interpreted by the user
/// as both 256-bit integers and 256-bit hashes.
struct evmc_uint256be {
    /// The 32 bytes of the big-endian integer or hash.
    uint8_t bytes[32];
};

/// Big-endian 160-bit hash suitable for keeping an Ethereum address.
struct evmc_address {
    /// The 20 bytes of the hash.
    uint8_t bytes[20];
};

/// The kind of call-like instruction.
enum evmc_call_kind {
    EVMC_CALL = 0,         ///< Request CALL.
    EVMC_DELEGATECALL = 1, ///< Request DELEGATECALL. The value param ignored.
    EVMC_CALLCODE = 2,     ///< Request CALLCODE.
    EVMC_CREATE = 3,       ///< Request CREATE. Semantic of some params changes.
};

/// The flags for ::evmc_message.
enum evmc_flags {
    EVMC_STATIC = 1        ///< Static call mode.
};

/// The message describing an EVM call,
/// including a zero-depth calls from a transaction origin.
struct evmc_message {
    struct evmc_address destination;  ///< The destination of the message.
    struct evmc_address sender;       ///< The sender of the message.

    /// The amount of Ether transferred with the message.
    struct evmc_uint256be value;

    /// The message input data.
    ///
    /// This MAY be NULL.
    const uint8_t* input_data;

    /// The size of the message input data.
    ///
    /// If input_data is NULL this MUST be 0.
    size_t input_size;

    /// The optional hash of the code of the destination account.
    /// The null hash MUST be used when not specified.
    struct evmc_uint256be code_hash;

    int64_t gas;                 ///< The amount of gas for message execution.
    int32_t depth;               ///< The call depth.

    /// The kind of the call. For zero-depth calls ::EVMC_CALL SHOULD be used.
    enum evmc_call_kind kind;

    /// Additional flags modifying the call execution behavior.
    /// In the current version the only valid values are ::EVMC_STATIC or 0.
    uint32_t flags;
};


/// The transaction and block data for execution.
struct evmc_tx_context {
    struct evmc_uint256be tx_gas_price;      ///< The transaction gas price.
    struct evmc_address tx_origin;           ///< The transaction origin account.
    struct evmc_address block_coinbase;      ///< The miner of the block.
    int64_t block_number;                   ///< The block number.
    int64_t block_timestamp;                ///< The block timestamp.
    int64_t block_gas_limit;                ///< The block gas limit.
    struct evmc_uint256be block_difficulty;  ///< The block difficulty.
};

struct evmc_context;

/// Get transaction context callback function.
///
/// This callback function is used by an EVM to retrieve the transaction and
/// block context.
///
/// @param[out] result   The returned transaction context.
///                      @see ::evmc_tx_context.
/// @param      context  The pointer to the Host execution context.
///                      @see ::evmc_context.
typedef void (*evmc_get_tx_context_fn)(struct evmc_tx_context* result,
                                       struct evmc_context* context);

/// Get block hash callback function.
///
/// This callback function is used by an EVM to query the block hash of
/// a given block.
///
/// @param[out] result   The returned block hash value.
/// @param      context  The pointer to the Host execution context.
/// @param      number   The block number. Must be a value between
//                       (and including) 0 and 255.
typedef void (*evmc_get_block_hash_fn)(struct evmc_uint256be* result,
                                       struct evmc_context* context,
                                       int64_t number);

/// The execution status code.
enum evmc_status_code {
    EVMC_SUCCESS = 0,               ///< Execution finished with success.
    EVMC_FAILURE = 1,               ///< Generic execution failure.
    EVMC_OUT_OF_GAS = 2,
    EVMC_UNDEFINED_INSTRUCTION = 3, ///< Unknown instruction encountered by the VM.
    EVMC_BAD_JUMP_DESTINATION = 4,
    EVMC_STACK_OVERFLOW = 5,
    EVMC_STACK_UNDERFLOW = 6,
    EVMC_REVERT = 7,                ///< Execution terminated with REVERT opcode.

    /// Tried to execute an operation which is restricted in static mode.
    EVMC_STATIC_MODE_VIOLATION = 8,

    /// The dedicated INVALID instruction was hit.
    EVMC_INVALID_INSTRUCTION = 9,

    /// Tried to read outside memory bounds.
    ///
    /// An example is RETURNDATACOPY reading past the available buffer.
    EVMC_INVALID_MEMORY_ACCESS = 10,

    /// Exceptions produced by precompiles/system contracts
    ///
    /// An example: elliptic curve functions handed invalid EC points
    EVMC_PRECOMPILE_FAILURE = 11,

    /// Call depth exceeded (if there is a call depth limit)
    EVMC_CALL_DEPTH_EXCEEDED = 12,

    /// Contract validation has failed (e.g. due to EVM 1.5 jump validity,
    /// Casper's purity checker or ewasm contract rules).
    EVMC_CONTRACT_VALIDATION_FAILURE = 13,

    /// The EVM rejected the execution of the given code or message.
    ///
    /// This error SHOULD be used to signal that the EVM is not able to or
    /// willing to execute the given code type or message.
    /// If an EVM returns the ::EVMC_REJECTED status code,
    /// the Client MAY try to execute it in other EVM implementation.
    /// For example, the Client tries running a code in the EVM 1.5. If the
    /// code is not supported there, the execution falls back to the EVM 1.0.
    EVMC_REJECTED = -1,

    /// EVM implementation internal error.
    ///
    /// @todo We should rethink reporting internal errors. One of the options
    ///       it to allow using any negative value to represent internal errors.
    EVMC_INTERNAL_ERROR = -2,
};

struct evmc_result;  ///< Forward declaration.

/// Releases resources assigned to an execution result.
///
/// This function releases memory (and other resources, if any) assigned to the
/// specified execution result making the result object invalid.
///
/// @param result  The execution result which resource are to be released. The
///                result itself it not modified by this function, but becomes
///                invalid and user should discard it as well.
typedef void (*evmc_release_result_fn)(const struct evmc_result* result);

/// The EVM code execution result.
struct evmc_result {
    /// The execution status code.
    enum evmc_status_code status_code;

    /// The amount of gas left after the execution.
    ///
    /// If evmc_result::code is not ::EVMC_SUCCESS nor ::EVMC_REVERT
    /// the value MUST be 0.
    int64_t gas_left;

    /// The reference to output data.
    ///
    /// The output contains data coming from RETURN opcode (iff evmc_result::code
    /// field is ::EVMC_SUCCESS) or from REVERT opcode.
    ///
    /// The memory containing the output data is owned by EVM and has to be
    /// freed with evmc_result::release().
    ///
    /// This MAY be NULL.
    const uint8_t* output_data;

    /// The size of the output data.
    ///
    /// If output_data is NULL this MUST be 0.
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
    /// struct evmc_result result = ...;
    /// if (result.release)
    ///     result.release(&result);
    /// @endcode
    ///
    /// @note
    /// It works similarly to C++ virtual destructor. Attaching the release
    /// function to the result itself allows EVM composition.
    evmc_release_result_fn release;

    /// The address of the contract created by CREATE opcode.
    ///
    /// This field has valid value only if the result describes successful
    /// CREATE (evmc_result::status_code is ::EVMC_SUCCESS).
    struct evmc_address create_address;

    /// Reserved data that MAY be used by a evmc_result object creator.
    ///
    /// This reserved 4 bytes together with 20 bytes from create_address form
    /// 24 bytes of memory called "optional data" within evmc_result struct
    /// to be optionally used by the evmc_result object creator.
    ///
    /// @see evmc_result_optional_data, evmc_get_optional_data().
    ///
    /// Also extends the size of the evmc_result to 64 bytes (full cache line).
    uint8_t padding[4];
};


/// The union representing evmc_result "optional data".
///
/// The evmc_result struct contains 24 bytes of optional data that can be
/// reused by the object creator if the object does not contain
/// evmc_result::create_address.
///
/// An EVM implementation MAY use this memory to keep additional data
/// when returning result from ::evmc_execute_fn.
/// The host application MAY use this memory to keep additional data
/// when returning result of performed calls from ::evmc_call_fn.
///
/// @see evmc_get_optional_data(), evmc_get_const_optional_data().
union evmc_result_optional_data
{
    uint8_t bytes[24];  ///< 24 bytes of optional data.
    void* pointer;      ///< Optional pointer.
};

/// Provides read-write access to evmc_result "optional data".
static inline union evmc_result_optional_data* evmc_get_optional_data(
    struct evmc_result* result)
{
    return (union evmc_result_optional_data*) &result->create_address;
}

/// Provides read-only access to evmc_result "optional data".
static inline const union evmc_result_optional_data* evmc_get_const_optional_data(
    const struct evmc_result* result)
{
    return (const union evmc_result_optional_data*) &result->create_address;
}


/// Check account existence callback function
///
/// This callback function is used by the EVM to check if
/// there exists an account at given address.
/// @param      context  The pointer to the Host execution context.
///                      @see ::evmc_context.
/// @param      address  The address of the account the query is about.
/// @return              1 if exists, 0 otherwise.
typedef int (*evmc_account_exists_fn)(struct evmc_context* context,
                                      const struct evmc_address* address);

/// Get storage callback function.
///
/// This callback function is used by an EVM to query the given contract
/// storage entry.
/// @param[out] result   The returned storage value.
/// @param      context  The pointer to the Host execution context.
///                      @see ::evmc_context.
/// @param      address  The address of the contract.
/// @param      key      The index of the storage entry.
typedef void (*evmc_get_storage_fn)(struct evmc_uint256be* result,
                                    struct evmc_context* context,
                                    const struct evmc_address* address,
                                    const struct evmc_uint256be* key);

/// Set storage callback function.
///
/// This callback function is used by an EVM to update the given contract
/// storage entry.
/// @param context  The pointer to the Host execution context.
///                 @see ::evmc_context.
/// @param address  The address of the contract.
/// @param key      The index of the storage entry.
/// @param value    The value to be stored.
typedef void (*evmc_set_storage_fn)(struct evmc_context* context,
                                    const struct evmc_address* address,
                                    const struct evmc_uint256be* key,
                                    const struct evmc_uint256be* value);

/// Get balance callback function.
///
/// This callback function is used by an EVM to query the balance of the given
/// address.
/// @param[out] result   The returned balance value.
/// @param      context  The pointer to the Host execution context.
///                      @see ::evmc_context.
/// @param      address  The address.
typedef void (*evmc_get_balance_fn)(struct evmc_uint256be* result,
                                    struct evmc_context* context,
                                    const struct evmc_address* address);

/// Get code size callback function.
///
/// This callback function is used by an EVM to get the size of the code stored
/// in the account at the given address. For accounts not having a code, this
/// function returns 0.
typedef size_t (*evmc_get_code_size_fn)(struct evmc_context* context,
                                        const struct evmc_address* address);

/// Copy code callback function.
///
/// This callback function is used by an EVM to request a copy of the code
/// of the given account to the memory buffer provided by the EVM.
/// The Client MUST copy the requested code, starting with the given offset,
/// to the provided memory buffer up to the size of the buffer or the size of
/// the code, whichever is smaller.
///
/// @param context      The pointer to the Client execution context.
///                          @see ::evmc_context.
/// @param address      The address of the account.
/// @param code_offset  The offset of the code to copy.
/// @param buffer_data  The pointer to the memory buffer allocated by the EVM
///                     to store a copy of the requested code.
/// @param buffer_size  The size of the memory buffer.
/// @return             The number of bytes copied to the buffer by the Client.
typedef size_t (*evmc_copy_code_fn)(struct evmc_context* context,
                                    const struct evmc_address* address,
                                    size_t code_offset,
                                    uint8_t* buffer_data,
                                    size_t buffer_size);

/// Selfdestruct callback function.
///
/// This callback function is used by an EVM to SELFDESTRUCT given contract.
/// The execution of the contract will not be stopped, that is up to the EVM.
///
/// @param context      The pointer to the Host execution context.
///                     @see ::evmc_context.
/// @param address      The address of the contract to be selfdestructed.
/// @param beneficiary  The address where the remaining ETH is going to be
///                     transferred.
typedef void (*evmc_selfdestruct_fn)(struct evmc_context* context,
                                     const struct evmc_address* address,
                                     const struct evmc_address* beneficiary);

/// Log callback function.
///
/// This callback function is used by an EVM to inform about a LOG that happened
/// during an EVM bytecode execution.
/// @param context       The pointer to the Host execution context.
///                      @see ::evmc_context.
/// @param address       The address of the contract that generated the log.
/// @param data          The pointer to unindexed data attached to the log.
/// @param data_size     The length of the data.
/// @param topics        The pointer to the array of topics attached to the log.
/// @param topics_count  The number of the topics. Valid values are between
///                      0 and 4 inclusively.
typedef void (*evmc_emit_log_fn)(struct evmc_context* context,
                                 const struct evmc_address* address,
                                 const uint8_t* data,
                                 size_t data_size,
                                 const struct evmc_uint256be topics[],
                                 size_t topics_count);

/// Pointer to the callback function supporting EVM calls.
///
/// @param[out] result  The result of the call. The result object is not
///                     initialized by the EVM, the Client MUST correctly
///                     initialize all expected fields of the structure.
/// @param      context The pointer to the Host execution context.
///                     @see ::evmc_context.
/// @param      msg     Call parameters. @see ::evmc_message.
typedef void (*evmc_call_fn)(struct evmc_result* result,
                             struct evmc_context* context,
                             const struct evmc_message* msg);

/// The context interface.
///
/// The set of all callback functions expected by EVM instances. This is C
/// realisation of vtable for OOP interface (only virtual methods, no data).
/// Host implementations SHOULD create constant singletons of this (similarly
/// to vtables) to lower the maintenance and memory management cost.
struct evmc_context_fn_table {

    /// Check account existence callback function.
    evmc_account_exists_fn account_exists;

    /// Get storage callback function.
    evmc_get_storage_fn get_storage;

    /// Set storage callback function.
    evmc_set_storage_fn set_storage;

    /// Get balance callback function.
    evmc_get_balance_fn get_balance;

    /// Get code size callback function.
    evmc_get_code_size_fn get_code_size;

    /// Copy code callback function.
    evmc_copy_code_fn copy_code;

    /// Selfdestruct callback function.
    evmc_selfdestruct_fn selfdestruct;

    /// Call callback function.
    evmc_call_fn call;

    /// Get transaction context callback function.
    evmc_get_tx_context_fn get_tx_context;

    /// Get block hash callback function.
    evmc_get_block_hash_fn get_block_hash;

    /// Emit log callback function.
    evmc_emit_log_fn emit_log;
};


/// Execution context managed by the Host.
///
/// The Host MUST pass the pointer to the execution context to
/// ::evmc_execute_fn. The EVM MUST pass the same pointer back to the Host in
/// every callback function.
/// The context MUST contain at least the function table defining the context
/// callback interface.
/// Optionally, The Host MAY include in the context additional data.
struct evmc_context {

    /// Function table defining the context interface (vtable).
    const struct evmc_context_fn_table* fn_table;
};


struct evmc_instance;  ///< Forward declaration.

/// Destroys the EVM instance.
///
/// @param evm  The EVM instance to be destroyed.
typedef void (*evmc_destroy_fn)(struct evmc_instance* evm);


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
typedef int (*evmc_set_option_fn)(struct evmc_instance* evm,
                                  char const* name,
                                  char const* value);


/// EVM revision.
///
/// The revision of the EVM specification based on the Ethereum
/// upgrade / hard fork codenames.
enum evmc_revision {
    EVMC_FRONTIER = 0,
    EVMC_HOMESTEAD = 1,
    EVMC_TANGERINE_WHISTLE = 2,
    EVMC_SPURIOUS_DRAGON = 3,
    EVMC_BYZANTIUM = 4,
    EVMC_CONSTANTINOPLE = 5,
};


/// Generates and executes machine code for given EVM bytecode.
///
/// All the fun is here. This function actually does something useful.
///
/// @param instance    A EVM instance.
/// @param context     The pointer to the Host execution context to be passed
///                    to callback functions. @see ::evmc_context.
/// @param rev         Requested EVM specification revision.
/// @param msg         Call parameters. @see ::evmc_message.
/// @param code        Reference to the bytecode to be executed.
/// @param code_size   The length of the bytecode.
/// @return            All execution results.
typedef struct evmc_result (*evmc_execute_fn)(struct evmc_instance* instance,
                                             struct evmc_context* context,
                                             enum evmc_revision rev,
                                             const struct evmc_message* msg,
                                             uint8_t const* code,
                                             size_t code_size);


/// The EVM instance.
///
/// Defines the base struct of the EVM implementation.
struct evmc_instance
{
    /// EVMC ABI version implemented by the EVM instance.
    ///
    /// Used to detect ABI incompatibilities. The EVMC ABI version
    /// represented by this file is in ::EVMC_ABI_VERSION.
    const int abi_version;

    /// The name of the EVMC VM implementation.
    ///
    /// It MUST be a NULL-terminated not empty string.
    const char* name;

    /// The version of the EVMC VM implementation, e.g. "1.2.3b4".
    ///
    /// It MUST be a NULL-terminated not empty string.
    const char* version;

    /// Pointer to function destroying the EVM instance.
    evmc_destroy_fn destroy;

    /// Pointer to function executing a code by the EVM instance.
    evmc_execute_fn execute;

    /// Optional pointer to function modifying VM's options.
    ///
    /// If the VM does not support this feature the pointer can be NULL.
    evmc_set_option_fn set_option;
};

// END Python CFFI declarations

/// Example of a function creating an instance of an example EVM implementation.
///
/// Each EVM implementation MUST provide a function returning an EVM instance.
/// The function SHOULD be named `evmc_create_<vm-name>(void)`.
///
/// @return  EVM instance or NULL indicating instance creation failure.
///
/// struct evmc_instance* evmc_create_examplevm(void);


#if __cplusplus
}
#endif

#endif  // EVMC_H
/// @}
