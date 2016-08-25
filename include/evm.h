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

/// Allow implementation to inject some additional information about function
/// linkage and/or symbol visibility in the output library.
#ifndef EXPORT
#define EXPORT
#endif

#if __cplusplus
extern "C" {
#endif

/// Host-endian 256-bit integer.
///
/// 32 bytes of data representing host-endian (that means little-endian almost
/// all the time) 256-bit integer. This applies to the words[] order as well.
/// words[0] contains the 64 lowest precision bits, words[3] constains the 64
/// highest precision bits.
struct evm_uint256 {
    /// The 4 64-bit words of the integer. Memory aligned to 8 bytes.
    uint64_t words[4];
};

/// Big-endian 160-bit hash suitable for keeping an Ethereum address.
struct evm_hash160 {
    /// The 20 bytes of the hash.
    uint8_t bytes[20];
};


/// Big-endian 256-bit integer/hash.
///
/// 32 bytes of data. For EVM that means big-endian 256-bit integer. Values of
/// this type are converted to host-endian values inside EVM.
struct evm_hash256 {
    union {
        /// The 32 bytes of the integer/hash. Memory aligned to 8 bytes.
        uint8_t bytes[32];
        /// Additional access by uint64 words to enforce 8 bytes alignment.
        uint64_t words[4];
    };
};

/// The execution result code.
enum evm_result_code {
    EVM_SUCCESS = 0,               ///< Execution finished with success.
    EVM_FAILURE = 1,               ///< Generic execution failure.
    EVM_OUT_OF_GAS = 2,
    EVM_BAD_INSTRUCTION = 3,
    EVM_BAD_JUMP_DESTINATION = 4,
    EVM_STACK_OVERFLOW = 5,
    EVM_STACK_UNDERFLOW = 6,
};

/// The EVM code execution result.
struct evm_result {
    /// The execution result code.
    enum evm_result_code code;

    /// The amount of gas left after the execution.
    ///
    /// The value is valid only if evm_result::code == ::EVM_SUCCESS.
    int64_t gas_left;

    /// The reference to output data. The memory containing the output data
    /// is owned by EVM and is freed with evm_release_result_fn().
    uint8_t const* output_data;

    /// The size of the output data.
    size_t output_size;

    /// @name Optional
    /// The optional information that EVM is not required to provide.
    /// @{

    /// The pointer to EVM-owned memory. For EVM internal use.
    /// @see output_data.
    void* internal_memory;

    /// The error message explaining the result code.
    char const* error_message;

    /// @}
};

/// The query callback key.
enum evm_query_key {
    EVM_SLOAD = 0,            ///< Storage value of a given key for SLOAD.
    EVM_ADDRESS = 1,          ///< Address of the contract for ADDRESS.
    EVM_CALLER = 2,           ///< Message sender address for CALLER.
    EVM_ORIGIN = 3,           ///< Transaction origin address for ORIGIN.
    EVM_GAS_PRICE = 4,        ///< Transaction gas price for GASPRICE.
    EVM_COINBASE = 5,         ///< Current block miner address for COINBASE.
    EVM_DIFFICULTY = 6,       ///< Current block difficulty for DIFFICULTY.
    EVM_GAS_LIMIT = 7,        ///< Current block gas limit for GASLIMIT.
    EVM_NUMBER = 8,           ///< Current block number for NUMBER.
    EVM_TIMESTAMP = 9,        ///< Current block timestamp for TIMESTAMP.
    EVM_CODE_BY_ADDRESS = 10, ///< Code by an address for EXTCODE/SIZE.
    EVM_BALANCE = 11,         ///< Balance of a given address for BALANCE.
    EVM_BLOCKHASH = 12        ///< Block hash of by block number for BLOCKHASH.
};


/// Opaque struct representing execution enviroment managed by the host
/// application.
struct evm_env;

/// Variant type to represent possible types of values used in EVM.
///
/// Type-safety is lost around the code that uses this type. We should have
/// complete set of unit tests covering all possible cases.
/// The size of the type is 64 bytes and should fit in single cache line.
union evm_variant {
    /// A host-endian 64-bit integer.
    int64_t int64;

    /// A host-endian 256-bit integer.
    struct evm_uint256 uint256;

    /// A big-endian 256-bit integer/hash.
    struct evm_hash256 hash256;

    struct {
        /// Additional padding to align the evm_variant::address with lower
        /// bytes of a full 256-bit hash.
        uint8_t address_padding[12];

        /// An Ethereum address.
        struct evm_hash160 address;
    };

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
/// about additional data required to execute EVM code.
/// @param env  Pointer to execution environment managed by the host
///             application.
/// @param key  The kind of the query. See evm_query_key and details below.
/// @param arg  Additional argument to the query. It has defined value only for
///             the subset of query keys.
///
/// ## Types of queries
/// Key                   | Arg                  | Expected result
/// ----------------------| -------------------- | ----------------------------
/// ::EVM_GAS_PRICE       |                      | evm_variant::uint256
/// ::EVM_ADDRESS         |                      | evm_variant::address
/// ::EVM_CALLER          |                      | evm_variant::address
/// ::EVM_ORIGIN          |                      | evm_variant::address
/// ::EVM_COINBASE        |                      | evm_variant::address
/// ::EVM_DIFFICULTY      |                      | evm_variant::uint256
/// ::EVM_GAS_LIMIT       |                      | evm_variant::uint256
/// ::EVM_NUMBER          |                      | evm_variant::int64?
/// ::EVM_TIMESTAMP       |                      | evm_variant::int64?
/// ::EVM_CODE_BY_ADDRESS | evm_variant::address | evm_variant::data
/// ::EVM_BALANCE         | evm_variant::address | evm_variant::uint256
/// ::EVM_BLOCKHASH       | evm_variant::int64   | evm_variant::hash256
/// ::EVM_SLOAD           | evm_variant::uint256 | evm_variant::uint256?
typedef union evm_variant (*evm_query_fn)(struct evm_env* env,
                                          enum evm_query_key key,
                                          union evm_variant arg);

/// The update callback key.
enum evm_update_key {
    EVM_SSTORE = 0,        ///< Update storage entry
    EVM_LOG = 1,           ///< Log.
    EVM_SELFDESTRUCT = 2,  ///< Mark contract as selfdestructed and set
                           ///  beneficiary address.
};


/// Update callback function.
///
/// This callback function is used by the EVM to modify contract state in the
/// host application.
/// @param env  Pointer to execution environment managed by the host
///             application.
/// @param key  The kind of the update. See evm_update_key and details below.
/// @param arg1 Additional argument to the update. It has defined value only for
///             the subset of update keys.
/// @param arg1 Additional argument to the update. It has defined value only for
///             the subset of update keys.
///
/// ## Types of updates
/// Key                   | Arg1                 | Arg2
/// ----------------------| -------------------- | --------------------
/// ::EVM_SSTORE          | evm_variant::uint256 | evm_variant::uint256
/// ::EVM_LOG             | evm_variant::data    | evm_variant::data
/// ::EVM_SELFDESTRUCT    | evm_variant::address | n/a
typedef void (*evm_update_fn)(struct evm_env* env,
                              enum evm_update_key key,
                              union evm_variant arg1,
                              union evm_variant arg2);

/// The kind of call-like instruction.
enum evm_call_kind {
    EVM_CALL = 0,         ///< Request CALL.
    EVM_DELEGATECALL = 1, ///< Request DELEGATECALL. The value param ignored.
    EVM_CALLCODE = 2,     ///< Request CALLCODE.
    EVM_CREATE = 3        ///< Request CREATE. Semantic of some params changes.
};

/// The flag indicating call failure in evm_call_fn().
static const int64_t EVM_CALL_FAILURE = INT64_MIN;

/// Pointer to the callback function supporting EVM calls.
///
/// @param env          Pointer to execution environment managed by the host
///                     application.
/// @param kind         The kind of call-like opcode requested.
/// @param gas          The amount of gas for the call.
/// @param address      The address of a contract to be called. Ignored in case
///                     of CREATE.
/// @param value        The value sent to the callee. The endowment in case of
///                     CREATE.
/// @param input        The call input data or the CREATE init code.
/// @param input_size   The size of the input data.
/// @param output       The reference to the memory where the call output is to
///                     be copied. In case of CREATE, the memory is guaranteed
///                     to be at least 20 bytes to hold the address of the
///                     created contract.
/// @param output_data  The size of the output data. In case of CREATE, expected
///                     value is 20.
/// @return      If non-negative - the amount of gas left,
///              If negative - an exception occurred during the call/create.
///              There is no need to set 0 address in the output in this case.
typedef int64_t (*evm_call_fn)(
    struct evm_env* env,
    enum evm_call_kind kind,
    int64_t gas,
    struct evm_hash160 address,
    struct evm_uint256 value,
    uint8_t const* input,
    size_t input_size,
    uint8_t* output,
    size_t output_size);


/// A piece of information about the EVM implementation.
enum evm_info_key {
    EVM_NAME  = 0,   ///< The name of the EVM implementation. ASCII encoded.
    EVM_VERSION = 1  ///< The software version of the EVM.
};

/// Request information about the EVM implementation.
/// FIXME: I don't think we need this, as we don't go towards fully dynamic
///        solution (DLLs, dlopen()). User should know a priori what he is
///        integrating with.
///
/// @param key  What do you want to know?
/// @return     Requested information as a c-string. Nonnull.
EXPORT char const* evm_get_info(enum evm_info_key key);

/// Opaque type representing a EVM instance.
struct evm_instance;

/// Creates new EVM instance.
///
/// Creates new EVM instance. The instance must be destroyed in evm_destroy().
/// Single instance is thread-safe and can be shared by many threads. Having
/// **multiple instances is safe but discouraged** as it has not benefits over
/// having the singleton.
///
/// @param query_fn   Pointer to query callback function. Nonnull.
/// @param update_fn  Pointer to update callback function. Nonnull.
/// @param call_fn    Pointer to call callback function. Nonnull.
/// @return           Pointer to the created EVM instance.
typedef struct evm_instance* (*evm_create_fn)(evm_query_fn query_fn,
                                              evm_update_fn update_fn,
                                              evm_call_fn call_fn);

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
/// @param name   The option name. Cannot be null.
/// @param value  The new option value. Cannot be null.
/// @return       1 if the option set successfully, 0 otherwise.
typedef int (*evm_set_option_fn)(struct evm_instance* evm,
                                 char const* name,
                                 char const* value);


/// EVM compatibility mode aka chain mode.
/// TODO: Can you suggest better name?
enum evm_mode {
    EVM_FRONTIER = 0,
    EVM_HOMESTEAD = 1
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
                                            struct evm_hash256 code_hash,
                                            uint8_t const* code,
                                            size_t code_size,
                                            int64_t gas,
                                            uint8_t const* input,
                                            size_t input_size,
                                            struct evm_uint256 value);

/// Releases resources assigned to an execution result.
///
/// This function releases memory (and other resources, if any) assigned to the
/// specified execution result making the result object invalid.
///
/// @param result  The execution result which resource are to be released. The
///                result itself it not modified by this function, but becomes
///                invalid and user should discard it as well.
typedef void (*evm_release_result_fn)(struct evm_result const* result);

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
                          struct evm_hash256 code_hash);

/// Request preparation of the code for faster execution. It is not required
/// to execute the code but allows compilation of the code ahead of time in
/// JIT-like VMs.
typedef void (*evm_prepare_code_fn)(struct evm_instance* instance,
                                    enum evm_mode mode,
                                    uint8_t const* code,
                                    size_t code_size,
                                    struct evm_hash256 code_hash);

/// VM interface.
///
/// Defines the implementation of EVM-C interface for a VM.
struct evm_interface {
    /// Pointer to function creating a VM's instance.
    evm_create_fn create;

    /// Pointer to function destroying a VM's instance.
    evm_destroy_fn destroy;

    /// Pointer to function execuing a code in a VM.
    evm_execute_fn execute;

    /// Pointer to function releasing an execution result.
    evm_release_result_fn release_result;

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

/// Example of a function exporting an interface for an example VM.
///
/// Each VM implementation is obligates to provided a function returning
/// VM's interface. The function has to be named as `<vm-name>_get_interface()`.
///
/// @return  VM interface
struct evm_interface examplevm_get_interface();


#if __cplusplus
}
#endif

#endif  // EVM_H
/// @}
