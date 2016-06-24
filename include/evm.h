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

#include <stdint.h>    // Definition of int64_t, uint64_t.
#include <stddef.h>    // Definition of size_t.
#include <stdbool.h>   // Definition of bool.

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

/// 160-bit hash suitable for keeping an Ethereum address.
struct evm_hash160 {
    /// The 20 bytes of the hash.
    char bytes[20];
};


/// Big-endian 256-bit integer/hash.
///
/// 32 bytes of data. For EVM that means big-endian 256-bit integer. Values of
/// this type are converted to host-endian values inside EVM.
struct evm_hash256 {
    union {
        /// The 32 bytes of the integer/hash. Memory aligned to 8 bytes.
        char bytes[32];
        /// Additional access by uint64 words to enforce 8 bytes alignment.
        uint64_t words[4];
    };
};

/// Reference to non-mutable memory.
struct evm_bytes_view {
    char const* bytes;  ///< Pointer the begining of the memory.
    size_t size;        ///< The memory size.
};

/// Reference to mutable memory.
struct evm_mutable_bytes_view {
    char* bytes;        ///< Pointer the begining of the mutable memory.
    size_t size;        ///< The memory size.
};

/// The EVM execution return code.
enum evm_return_code {
    EVM_RETURN = 0,        ///< The execution ended by STOP or RETURN.
    EVM_SELFDESTRUCT = 1,  ///< The execution ended by SELFDESTRUCT.
    EVM_EXCEPTION = -1,    ///< The execution ended with an exception.
};

/// Complex struct representing execution result.
struct evm_result {
    /// Success? OOG? Selfdestruction?
    enum evm_return_code return_code;
    union {
        /// In case of successful execution this substruct is filled.
        struct {
            /// Rerefence to output data. The memory containing the output data
            /// is owned by EVM and is freed with evm_destroy_result().
            struct evm_bytes_view output_data;

            /// Gas left after execution. Non-negative.
            /// @todo We could squeeze gas_left and return_code together.
            int64_t gas_left;

            /// Pointer to EVM-owned memory.
            /// @see output_data.
            void* internal_memory;
        };
        /// In case of selfdestruction here is the address where money goes.
        struct evm_hash160 selfdestruct_beneficiary;
    };
};

/// The query callback key.
enum evm_query_key {
    EVM_ADDRESS,         ///< Address of the contract for ADDRESS.
    EVM_CALLER,          ///< Message sender address for CALLER.
    EVM_ORIGIN,          ///< Transaction origin address for ORIGIN.
    EVM_GAS_PRICE,       ///< Transaction gas price for GASPRICE.
    EVM_COINBASE,        ///< Current block miner address for COINBASE.
    EVM_DIFFICULTY,      ///< Current block difficulty for DIFFICULTY.
    EVM_GAS_LIMIT,       ///< Current block gas limit for GASLIMIT.
    EVM_NUMBER,          ///< Current block number for NUMBER.
    EVM_TIMESTAMP,       ///< Current block timestamp for TIMESTAMP.
    EVM_CODE_BY_ADDRESS, ///< Code by an address for EXTCODE/SIZE.
    EVM_BALANCE,         ///< Balance of a given address for BALANCE.
    EVM_STORAGE,         ///< Storage value of a given key for SLOAD.
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

    struct {
        /// Additional padding to align the evm_variant::address with lower
        /// bytes of a full 256-bit hash.
        char address_padding[12];

        /// An Ethereum address.
        struct evm_hash160 address;
    };

    /// A memory reference.
    struct evm_bytes_view bytes;
};

/// Query callback function.
///
/// This callback function is used by the EVM to query the host application
/// about additional data required to execute EVM code.
/// @param env  Pointer to execution environment managed by the host
/// application.
/// @param key  The kind of the query. See evm_query_key and details below.
/// @param arg  Additional argument to the query. It has defined value only for
///             the subset of query keys.
///
/// ## Types of queries
/// Key                   | Arg                  | Expected result
/// ----------------------| -------------------- | ----------------------------------
/// ::EVM_GAS_PRICE       |                      | evm_variant::uint256
/// ::EVM_ADDRESS         |                      | evm_variant::address
/// ::EVM_CALLER          |                      | evm_variant::address
/// ::EVM_ORIGIN          |                      | evm_variant::address
/// ::EVM_COINBASE        |                      | evm_variant::address
/// ::EVM_DIFFICULTY      |                      | evm_variant::uint256
/// ::EVM_GAS_LIMIT       |                      | evm_variant::int64
/// ::EVM_NUMBER          |                      | evm_variant::int64?
/// ::EVM_TIMESTAMP       |                      | evm_variant::int64?
/// ::EVM_CODE_BY_ADDRESS | evm_variant::address | evm_variant::bytes
/// ::EVM_BALANCE         | evm_variant::address | evm_variant::uint256
/// ::EVM_STORAGE         | evm_variant::uint256 | evm_variant::uint256?
typedef union evm_variant (*evm_query_fn)(struct evm_env* env,
                                          enum evm_query_key key,
                                          union evm_variant arg);


/// Callback function for modifying the storage.
///
/// Endianness: host-endianness is used because C++'s storage API uses big ints,
///             not bytes. What do you use?
typedef void (*evm_store_storage_fn)(struct evm_env* env,
                                     struct evm_uint256 key,
                                     struct evm_uint256 value);

/// The kind of call-like instruction.
enum evm_call_kind {
    EVM_CALL,         ///< Request CALL.
    EVM_DELEGATECALL, ///< Request DELEGATECALL. The value param ignored.
    EVM_CALLCODE,     ///< Request CALLCODE.
    EVM_CREATE        ///< Request CREATE. Semantic of some params changes.
};

/// Pointer to the callback function supporting EVM calls.
///
/// @param kind         The kind of call-like opcode requested.
/// @param gas          The amount of gas for the call.
/// @param address      The address of a contract to be called. Ignored in case
///                     of CREATE.
/// @param value        The value sent to the callee. The endowment in case of
///                     CREATE.
/// @param input_data   The call input data or the create init code.
/// @param output_data  The reference to the memory where the call output is to
///                     be copied. In case of create, the memory is guaranteed
///                     to be at least 160 bytes to hold the address of the
///                     created contract.
/// @return      If non-negative - the amount of gas left,
///              If negative - an exception occurred during the call/create.
///              There is no need to set 0 address in the output in this case.
typedef int64_t (*evm_call_fn)(
    enum evm_call_kind kind,
    int64_t gas,
    struct evm_hash160 address,
    struct evm_uint256 value,
    struct evm_bytes_view input_data,
    struct evm_mutable_bytes_view output_data);

/// Pointer to the callback function supporting EVM logs.
///
/// @param log_data    Reference to memory containing non-indexed log data.
/// @param num_topics  Number of topics added to the log. Valid values 0-4.
/// @param topics      Pointer to an array containing `num_topics` topics.
typedef void (*evm_log_fn)(struct evm_bytes_view log_data,
                           size_t num_topics,
                           struct evm_hash256 topics[]);


/// A piece of information about the EVM implementation.
enum evm_info_key {
    EVM_NAME,     ///< The name of the EVM implementation.
    EVM_VERSION   ///< The software version of the EVM.
};

/// Request information about the EVM implementation.
///
/// @param key  What do you want to know?
/// @return     Requested information as a c-string. Nonnull.
char const* evm_get_info(enum evm_info_key key);

/// Opaque type representing a EVM instance.
struct evm_instance;

/// Creates new EVM instance.
///
/// Creates new EVM instance. The instance must be destroyed in evm_destroy().
/// Single instance is thread-safe and can be shared by many threads. Having
/// **multiple instances is safe but discouraged** as it has not benefits over
/// having the singleton.
///
/// @param query_fn    Pointer to query callback function. Nonnull.
/// @param storage_fn  Pointer to storage callback function. Nonnull.
/// @param call_fn     Pointer to call callback function. Nonnull.
/// @param log_fn      Pointer to log callback function. Nonnull.
/// @return            Pointer to the created EVM instance.
struct evm_instance* evm_create(evm_query_fn query_fn,
                                evm_store_storage_fn storage_fn,
                                evm_call_fn call_fn,
                                evm_log_fn log_fn);

/// Destroys the EVM instance.
///
/// @param evm  The EVM instance to be destroyed.
void evm_destroy(struct evm_instance* evm);


/// Configures the EVM instance.
///
/// Allows modifying options of the EVM instance.
/// Options:
/// - compatibility mode: frontier, homestead, metropolis, ...
/// - code cache behavior: on, off, read-only, ...
/// - optimizations,
///
/// @param evm    The EVM instance to be configured.
/// @param name   The option name. Cannot be null.
/// @param value  The new option value. Cannot be null.
/// @return       True if the option set successfully.
bool evm_set_option(struct evm_instance* evm,
                    char const* name,
                    char const* value);


/// Generates and executes machine code for given EVM bytecode.
///
/// All the fun is here. This function actually does something useful.
///
/// @param instance    A EVM instance.
/// @param env         A pointer to the execution environment provided by the
///                    user and passed to callback functions.
/// @param code_hash   A hash of the bytecode, usually Keccak. The EVM uses it
///                    as the code identifier. A EVM implementation is able to
///                    hash the code itself if it requires it, but the host
///                    application usually has the hash already.
/// @param code        Reference to the bytecode to be executed.
/// @param gas         Gas for execution. Min 0, max 2^63-1.
/// @param input_data  Reference to the call input data.
/// @param value       Call value.
/// @return            All execution results.
struct evm_result evm_execute(struct evm_instance* instance,
                              struct evm_env* env,
                              struct evm_hash256 code_hash,
                              struct evm_bytes_view code,
                              int64_t gas,
                              struct evm_bytes_view input_data,
                              struct evm_uint256 value);

/// Destroys execution result.
void evm_destroy_result(struct evm_result);


#if __cplusplus
}
#endif
/// @}
