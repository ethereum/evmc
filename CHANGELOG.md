# Changelog

Documentation of all notable changes to the **EVMC** project.

The format is based on [Keep a Changelog],
and this project adheres to [Semantic Versioning].

## [10.1.0] — 2023-04-22

### Added

- The Prague EVM revision (anticipated after Cancun)
  [#683](https://github.com/ethereum/evmc/pull/683)
- C++: Make `evmc::Result` accessible via `evmc_result` reference
  [#686](https://github.com/ethereum/evmc/pull/686)

### Changed

- Set Shanghai as the latest stable revision `EVMC_LATEST_STABLE_REVISION`.
  [#687](https://github.com/ethereum/evmc/pull/687)
- Bump minimum supported compiler versions:
  [#685](https://github.com/ethereum/evmc/pull/685)
  - GCC: 7 → 8
  - Clang: 5 → 9
  - CMake: 3.10 → 3.16


## [10.0.0] — 2022-08-25

### Added

- Information about `PUSH0` instruction from [EIP-3855](https://eips.ethereum.org/EIPS/eip-3855)
  for Shanghai revision.
  [#628](https://github.com/ethereum/evmc/pull/628)
- The [Paris](https://github.com/ethereum/execution-specs/blob/master/network-upgrades/mainnet-upgrades/paris.md)
  (aka The Merge) EVM revision.
  [#627](https://github.com/ethereum/evmc/pull/627)
  [#634](https://github.com/ethereum/evmc/pull/634)
- The Cancun EVM revision (anticipated after Shanghai)
  [#633](https://github.com/ethereum/evmc/pull/633)
- The gas refund counter has been added to the `evmc_result`.
  [#666](https://github.com/ethereum/evmc/pull/666)
- The error code `EVMC_LOADER_UNSPECIFIED_ERROR` has been defined to provide
  a convenient way of initializing `evmc_loader_error_code` objects.
  [#617](https://github.com/ethereum/evmc/pull/617)
- Support for Visual Studio 2022.
  [#619](https://github.com/ethereum/evmc/pull/619)
- C++ types `evmc::address` and `evmc::bytes32` are convertible to `std::basic_string_view<uint8_t>`.
  [#636](https://github.com/ethereum/evmc/pull/636)
- Convenient constructors for C++ `evmc::result`.
  [#660](https://github.com/ethereum/evmc/pull/660)
- Rust: The `EvmcVm::set_option` has been added.
  [#614](https://github.com/ethereum/evmc/pull/614)

### Changed

- The `code_address` field has been added to the `evmc_message` type.
  It represents the address of an account from which the code is being executed
  and is useful for `DELEGATECALL` implementations.
  [#611](https://github.com/ethereum/evmc/pull/611)
  [#615](https://github.com/ethereum/evmc/pull/615)
- The `evmc_message::destination` field has been renamed to `evmc_message::recipient`
  to clarify its purpose and match the naming from the Yellow Paper.
  [#616](https://github.com/ethereum/evmc/pull/616)
- The `evmc_storage_status` has been extended to provide information about every possible case of
  storage net gas metering ([EIP-2200](https://eips.ethereum.org/EIPS/eip-2200)).
  [#661](https://github.com/ethereum/evmc/pull/661)
- The `selfdestruct` method returns the information if the given address
  has not been registered as selfdestructed yet.
  [#662](https://github.com/ethereum/evmc/pull/662)
- C++: The `evmc::result` has been renamed to `evmc::Result` for consistency
  with C++ types of similar kind.
  [#665](https://github.com/ethereum/evmc/pull/665)
- C++: The `HostContext` does not cache transaction context (`evmc_tx_context`) anymore.
  [#631](https://github.com/ethereum/evmc/pull/631)
- Go: The `create2Salt` parameter has been removed from the `VM.Execute()`.
  [#612](https://github.com/ethereum/evmc/pull/612)
- Code quality improvements.
  [#618](https://github.com/ethereum/evmc/pull/618)
  [#620](https://github.com/ethereum/evmc/pull/620)
  [#621](https://github.com/ethereum/evmc/pull/621)
  [#632](https://github.com/ethereum/evmc/pull/632)
- According to [EIP-4399](https://eips.ethereum.org/EIPS/eip-4399),
  `block_difficulty` field was renamed to `block_prev_randao`, and `DIFFICULTY` opcode to `PREVRANDAO`.
  [#635](https://github.com/ethereum/evmc/pull/635)
- The `evmc::hex` support C++ library has been refactored and converted to a single-header library.
  [#643](https://github.com/ethereum/evmc/pull/643)
  [#648](https://github.com/ethereum/evmc/pull/648)
  [#649](https://github.com/ethereum/evmc/pull/649)
  [#654](https://github.com/ethereum/evmc/pull/654)
- For command-line tools to load input/code from a file the `@file` syntax must be used.
  E.g. `evmc run @contract.evm --input @data.in`.
  [#647](https://github.com/ethereum/evmc/pull/647)
- Improvements to the `evmc::MockedHost` testing utility around account storage and selfdestructs.
  [#661](https://github.com/ethereum/evmc/pull/661)
  [#662](https://github.com/ethereum/evmc/pull/662)
  [#670](https://github.com/ethereum/evmc/pull/670)

### Fixed

- Java bindings fixes.
  [#653](https://github.com/ethereum/evmc/pull/653)

### Removed

- The support for C++ `0_address` and `0_bytes32` literals has been removed.
  [#652](https://github.com/ethereum/evmc/pull/652)

## [9.0.0] — 2021-06-30

### Added

- Support for **London** [EIP-3198](https://eips.ethereum.org/EIPS/eip-3198):
  `block_base_fee` member added to `evmc_tx_context`,
  `BASEFEE` instruction added.
  [#603](https://github.com/ethereum/evmc/pull/603)

- Added **Shanghai** EVM revision.
  [#604](https://github.com/ethereum/evmc/pull/604)

- Added [`EVMC_LATEST_STABLE_REVISON`](https://evmc.ethereum.org/group__EVMC.html#ggae5759b1590071966ccf6a505b52a0ef7a0dd8bdd55816359290e8fb8648aeb03e):
  alias for the latest known EVM revision with finalized specification.
  [#605](https://github.com/ethereum/evmc/pull/605)

- The `--bench` flag has been added to `evmc run` to more precisely measure execution time.
  [#598](https://github.com/ethereum/evmc/pull/598)

- Output stream operators and `to_string()` overloads for EVMC types
  `evmc_revision` and `evmc_status_code` are now part of public C and C++ API.
  [#599](https://github.com/ethereum/evmc/pull/599)

### Changed

- Adjustments to `evmc::mocked_host`: storage changes to non-existent keys are now preserved.
  [#594](https://github.com/ethereum/evmc/pull/594)

## [8.0.0] — 2021-04-27

### Added

- Support for **Berlin** [EIP-2929](https://eips.ethereum.org/EIPS/eip-2929):
  `access_account()` and `access_storage()` functions added to `evmc_host_interface`.
  [#571](https://github.com/ethereum/evmc/pull/571)

### Changed

- Instruction `SHA3` has been renamed to `KECCAK256` as proposed by
  [EIP-1803](https://eips.ethereum.org/EIPS/eip-1803) to better match the underlying hash function.
  [#590](https://github.com/ethereum/evmc/pull/590)
- The C++ standard required has been increased to **C++17**. The minimal officially supported Clang
  version has been increased from 3.8 to 5.
  [#584](https://github.com/ethereum/evmc/pull/584)


## [7.5.0] — 2021-03-23

### Added

- New option `--input` for `evmc run` tool to specify execution input data (_calldata_).
  [#564](https://github.com/ethereum/evmc/pull/564)
- New option `--create` for `evmc run` tool to create new contract with provided init code before 
  main execution. This allows using Solidity compiler binary outputs directly by the tool.
  [#566](https://github.com/ethereum/evmc/pull/566)
  ```bash
  solc --bin Contract.sol -o .
  evmc run --create Contract.bin --input 370158ea
  ```
- `evmc run` tool accepts both hex-strings or file paths for `code` and 
  `--input` arguments.
  [#574](https://github.com/ethereum/evmc/pull/574)
- New static C++ library `evmc::hex` added with procedures for hex encoding/decoding. 
  Hex-strings are used by EVMC and related projects for internal testing.
  [#575](https://github.com/ethereum/evmc/pull/575)
- New `EVMC_INSUFFICIENT_BALANCE` error code has been registered.
  [#528](https://github.com/ethereum/evmc/pull/528)

### Changed

- Java bindings fixes and improvements. It must be noted the bindings are in a _work in progress_
  state and are not suggested for production use.
  [#535](https://github.com/ethereum/evmc/pull/535)
  [#537](https://github.com/ethereum/evmc/pull/537)
  [#541](https://github.com/ethereum/evmc/pull/541)
  [#545](https://github.com/ethereum/evmc/pull/545)
  [#549](https://github.com/ethereum/evmc/pull/549)
  [#550](https://github.com/ethereum/evmc/pull/550)
  [#551](https://github.com/ethereum/evmc/pull/551)
  [#552](https://github.com/ethereum/evmc/pull/552)
  [#553](https://github.com/ethereum/evmc/pull/553)
  [#557](https://github.com/ethereum/evmc/pull/557)
  [#579](https://github.com/ethereum/evmc/pull/579)
  [#580](https://github.com/ethereum/evmc/pull/580)
  [#581](https://github.com/ethereum/evmc/pull/581)
- C++'s `std::hash` and comparison operators for EVMC types has been optimized.
  [#560](https://github.com/ethereum/evmc/pull/5601)
  [#561](https://github.com/ethereum/evmc/pull/561)
- `evmc --version` now also informs about the version of the loaded EVM.
  [#567](https://github.com/ethereum/evmc/pull/567)
- The Example VM (`evmc::example-vm`) has been converted from C to C++. It now implements a subset
  of real EVM opcodes so examples and tests can use valid EVM bytecodes.
  [#539](https://github.com/ethereum/evmc/pull/539)

## [7.4.0] — 2020-06-24

### Changed

- The C++ standard required has been increased to **C++14**.
  [#521](https://github.com/ethereum/evmc/pull/521)
- The C++ literals for `address` and `bytes32` types have been reimplemented to
  use `static_assert` to report errors. This makes C++ bindings exception-free
  (_no-throw guarantee_ level of exception safety).
  [#520](https://github.com/ethereum/evmc/pull/520)

### Removed

- The support for **Visual Studio 2015** C/C++ compiler has been dropped 
  as a consequence of requiring C++14.
  [#521](https://github.com/ethereum/evmc/pull/521)

## [7.3.0] — 2020-05-20

### Added

- Support for **Go modules**.
  [#486](https://github.com/ethereum/evmc/pull/486)

### Changed

- The minimum **Go version** supported bumped to **1.11** (Go modules are required).
  [#486](https://github.com/ethereum/evmc/pull/486)
- Removed dependency on go-ethereum in Go bindings by introducing own `Address` and `Hash` types.
  [#513](https://github.com/ethereum/evmc/pull/513)


## [7.2.0] — 2020-05-13

### Added

- Added **Java** bindings.
  [#455](https://github.com/ethereum/evmc/pull/455)
  [#490](https://github.com/ethereum/evmc/pull/490)
  [#503](https://github.com/ethereum/evmc/pull/503)
  [#512](https://github.com/ethereum/evmc/pull/512)
- New **evmc command-line tool** has been added. At the moment it supports
  command _run_ for executing bytecode in any EVMC-compatible VM implementation.
  Try `evmc run --help` for more information.
- C++: EVMC basic types `address` and `bytes32` have all the comparison operators supported.
  [#474](https://github.com/ethereum/evmc/pull/474)
- C++: Convenient constructors from `uint64_t` added for basic types `address` and `bytes32`.
  [#488](https://github.com/ethereum/evmc/pull/488)
- C++: Added `VM::get_raw_pointer()` method to directly access VM C API when needed.
  [#492](https://github.com/ethereum/evmc/pull/492)

### Changed

- CMake minimum version required has been bumped to [3.10](https://cmake.org/cmake/help/v3.10/release/3.10.html).
  [#495](https://github.com/ethereum/evmc/pull/495)

### Fixed

- The implementation of C++ `operator<` for `evmc::address` and `evmc::bytes32` has been fixed.
  [#498](https://github.com/ethereum/evmc/pull/498)


## [7.1.0] — 2019-11-29

### Added

- Added `MockedHost` C++ class (in form of header-only `evmc::mocked_host` library)
  which can be used to emulate Host behavior when testing VM implementations.
  [#456](https://github.com/ethereum/evmc/pull/456)
- In the Rust bindings added more type aliases (`MessageKind`, `MessageFlags`, `StatusCode`,
  `StorageStatus`, `Revision`).
  [#206](https://github.com/ethereum/evmc/pull/206)
- In CMake the `evmc::evmc_cpp` target has been added which represents the C++ EVMC API.
  [#470](https://github.com/ethereum/evmc/pull/470)

### Changed

- Require Rust 1.37.0 as a minimum.
- In the Rust bindings mark read-only functions in `ExecutionContext` as non-mutating.
  [#444](https://github.com/ethereum/evmc/pull/444)
- In the C++ `HostInterface` the logically read-only methods are marked with `const`.
  [#461](https://github.com/ethereum/evmc/pull/461)
- Updated dependencies of the Rust bindings to latest stable versions.
  [#462](https://github.com/ethereum/evmc/pull/462)

## [7.0.0] „Istanbul Ready” — 2019-11-11

This version of EVMC delivers compatibility with Istanbul EVM revision.
The ABI breaking change has been required in this case so some other 
backward-incompatible changes are also included and deprecated APIs have been 
removed.

### Added

- Support for **Istanbul** EIP-1344 (CHAINID opcode). 
  `chain_id` added to `evmc_tx_context` struct.
  [#375](https://github.com/ethereum/evmc/pull/375)
- Support for **Istanbul** EIP-1884 (Repricing for trie-size-dependent opcodes).
  [#372](https://github.com/ethereum/evmc/pull/372)
- The **Berlin** EVM revision number has been added.
  [#407](https://github.com/ethereum/evmc/pull/407)
- In C++ API, an overload for `VM::execute()` has been added that omits
  the Host context and interface parameters. This is useful for Precompiles VMs
  that do not interact with the Host.
  [#302](https://github.com/ethereum/evmc/pull/302)
- In C++ API, the `VM::has_capability()` method has been added.
  [#465](https://github.com/ethereum/evmc/pull/465)
  
### Changed

- The `evmc_instance` renamed to `evmc_vm`.
  [#430](https://github.com/ethereum/evmc/pull/430)
- The `evmc_context` renamed to `evmc_host_context`.
  [#426](https://github.com/ethereum/evmc/pull/426)
- The `evmc_host_interface` is now separated from `evmc_host_context`. 
  This simplifies language bindings which implement the `evmc_host_interface`.
  [#427](https://github.com/ethereum/evmc/pull/427)
- The `evmc::vm` renamed to `evmc::VM` in C++ API.
  [#252](https://github.com/ethereum/evmc/pull/252)
- Previously deprecated `helpers.hpp` header file has been removed.
  [#410](https://github.com/ethereum/evmc/pull/410)
- Previously deprecated `EVMC_CONSTANTINOPLE2` and `EVMC_LATEST_REVISION` revisions have been removed.
  [#411](https://github.com/ethereum/evmc/pull/411)
- Previously deprecated tracing API has been removed.
  [#429](https://github.com/ethereum/evmc/pull/429)
- In `evmc::instructions` library the undefined instructions have `0` gas cost
  instead of previous `-1` value.
  [#425](https://github.com/ethereum/evmc/pull/425)
- The EVM instruction tables have been redesigned to be more useful.
  [#435](https://github.com/ethereum/evmc/pull/435)
- The EVMC loader trims all extensions (previously only the last one)
  from the EVMC module file name.
  [#439](https://github.com/ethereum/evmc/pull/439)
- The EVMC loader no longer ties to guess the VM create function name 
  by dropping prefix words from the name.
  [#440](https://github.com/ethereum/evmc/pull/440)
- The helper function `evmc_is_abi_compatible()` returns now `bool`
  instead of `int`.
  [#442](https://github.com/ethereum/evmc/pull/442)
- In the Rust bindings make `ExecutionContext` optional within `execute`.
  [#350](https://github.com/ethereum/evmc/pull/350)
- A set of small improvements to C++ API.
  [#445](https://github.com/ethereum/evmc/pull/445)
  [#449](https://github.com/ethereum/evmc/pull/449)
  [#451](https://github.com/ethereum/evmc/pull/451)



## [6.3.1] - 2019-08-19

### Added

- Added `LoadAndConfigure` method to the Go API.
  [#404](https://github.com/ethereum/evmc/pull/404)
  
### Deprecated

- Previously deprecated `is_zero()` helper has been removed, 
  but replaced with new `evmc::is_zero()` in API compatible way.
  [#406](https://github.com/ethereum/evmc/pull/406)

### Fixed

- In C++ API the `get_balance()` method now returns expected `evmc::uint256be` 
  instead of `evmc_uint256be`.
  [#403](https://github.com/ethereum/evmc/pull/403)
- [Cable] upgraded to 0.4.4 to fix incompatibility with older versions.
  [#405](https://github.com/ethereum/evmc/pull/405)

## [6.3.0] - 2019-08-12

### Added

- Experimental support for _Precompiles_ - 
  EVMC modules containing implementations of Ethereum precompiled contracts.
  To learn more read
  the [EVMC Precompiles](https://github.com/ethereum/evmc/issues/259) feature description.
  [#267](https://github.com/ethereum/evmc/pull/267)
- The `vm::get_capabilities()` method has been added in C++ API.
  [#301](https://github.com/ethereum/evmc/pull/301)
- A CMake helper for running evmc-vmtester.
  [#303](https://github.com/ethereum/evmc/pull/303)
- The loader module introduces standardized EVMC module configuration string 
  which contains path to the module and additional options.
  E.g. `./modules/vm.so,engine=compiler,trace,verbosity=2`.
  A VM can be loaded, created and configured atomically with 
  new `evmc_load_and_configure()` function.
  [#313](https://github.com/ethereum/evmc/pull/313)
- Full support for 32-bit architectures has been added.
  [#327](https://github.com/ethereum/evmc/pull/327)
- The C/C++ API for creating execution results in VMs has been extended 
  to handle common usage cases.
  [#333](https://github.com/ethereum/evmc/pull/333)
- Support for moving `evmc::vm` objects in C++ API.
  [#341](https://github.com/ethereum/evmc/pull/341)
- The basic types `address` and `bytes32` have received their C++ wrappers 
  to assure they are always initialized. 
  They also have convenient overloaded operators for comparison 
  and usage as keys in standard containers.
  [#357](https://github.com/ethereum/evmc/pull/357)
- The C++ EVMC basic types `address` and `bytes32` have user defined literals.
  [#359](https://github.com/ethereum/evmc/pull/359)
  ```cpp
  auto a = 0xfB6916095ca1df60bB79Ce92cE3Ea74c37c5d359_address;
  auto b = 0xd4e56740f876aef8c010b86a40d5f56745a118d0906a34e69aec8c0db1cb8fa3_bytes32;
  ```
- CMake option `EVMC_INSTALL` (`ON` by default) to disable installing EVMC targets.
  This may be useful when EVMC is included in other project as git submodule or subtree.
  [#360](https://github.com/ethereum/evmc/pull/360)
- The `evmc-vmtester` tool received support for EVMC module configuration.
  E.g. `evmc-vmtester ./my_vm,mode=interpreter`.
  [#366](https://github.com/ethereum/evmc/pull/366)
- In `evmc-vm` Rust crate, `ExecutionResult` now has `success`, `failure` and `revert` helpers.
  [#297](https://github.com/ethereum/evmc/pull/297)
  [#368](https://github.com/ethereum/evmc/pull/368)
- Introduction of `evmc-declare` Rust crate with a procedural macro for easy VM declaration.
  [#262](https://github.com/ethereum/evmc/pull/262)
  [#316](https://github.com/ethereum/evmc/pull/316)
- Introduction of `ExecutionMessage` wrapper in the `evmc-vm` Rust crate.
  [#324](https://github.com/ethereum/evmc/pull/324)
- Added type aliases and traits on basic types in the `evmc-vm` Rust crate.
  [#342](https://github.com/ethereum/evmc/pull/342)
  [#343](https://github.com/ethereum/evmc/pull/343)

### Changed

- A lot of documentation fixes, improvements and cleanups.
  [#271](https://github.com/ethereum/evmc/pull/271)
  [#272](https://github.com/ethereum/evmc/pull/272)
  [#276](https://github.com/ethereum/evmc/pull/276)
  [#280](https://github.com/ethereum/evmc/pull/280)
  [#345](https://github.com/ethereum/evmc/pull/345)
- In C++ API `evmc::result::raw()` renamed to `evmc::result::release_raw()`.
  [#293](https://github.com/ethereum/evmc/pull/293)
- In `evmc_load_and_create()` the `error_code` is optional (can be `NULL`).
  [#311](https://github.com/ethereum/evmc/pull/311)

### Deprecated
  
- The usage of `evmc/helpers.hpp` has been deprecated. Use `evmc/evmc.hpp`
  which provides the same features.
  [#358](https://github.com/ethereum/evmc/pull/358)
- The tracing API has been deprecated as there have been some design flaws discovered. 
  New API is expected to be introduced in future.
  [#376](https://github.com/ethereum/evmc/pull/376)

### Fixed

- The `vmtester` tool now builds with MSVC with `/std:c++17`.
  [#261](https://github.com/ethereum/evmc/issues/261)
  [#263](https://github.com/ethereum/evmc/pull/263)
- The `evmc_release_result()` helper has been fixed.
  [#266](https://github.com/ethereum/evmc/issues/266)
  [#279](https://github.com/ethereum/evmc/pull/279)
- The missing include guard in `evmc.hpp` has been fixed.
  [#300](https://github.com/ethereum/evmc/pull/300)
- A loaded VM with incompatible ABI version is now properly destroyed.
  [#305](https://github.com/ethereum/evmc/issues/305)
  [#306](https://github.com/ethereum/evmc/pull/306)


## [6.2.2] - 2019-05-16

### Fixed

- Compilation error of `evmc::result::raw()` in Visual Studio fixed.
  [#281](https://github.com/ethereum/evmc/pull/281)
- The `evmc::result`'s move assignment operator fixed.
  [#282](https://github.com/ethereum/evmc/pull/282)

## [6.2.1] - 2019-04-29

### Fixed

- Disallow implicit conversion from C++ `evmc::result` to `evmc_result` 
  causing unintendent premature releasing of resources.
  [#256](https://github.com/ethereum/evmc/issues/256)
  [#257](https://github.com/ethereum/evmc/issues/257)


## [6.2.0] - 2019-04-25

### Added

- CMake option `EVMC_TEST_TOOLS` to build evmc-vmtester without bothering with internal unit tests.
  [#216](https://github.com/ethereum/evmc/pull/216)
- The full C++ EVMC API for both VM and Host implementations.
  [#217](https://github.com/ethereum/evmc/pull/217)
  [#226](https://github.com/ethereum/evmc/pull/226)
- Initial and rough **bindings for Rust**.  It is possible to implement an
  EVMC VM in Rust utilising some helpers.
  [#201](https://github.com/ethereum/evmc/pull/201)
  [#202](https://github.com/ethereum/evmc/pull/202)
  [#233](https://github.com/ethereum/evmc/pull/233)
- Handling of DLL loading errors greatly improved by 
  new `evmc_last_error_msg()` function.
  [#230](https://github.com/ethereum/evmc/pull/230)
  [#232](https://github.com/ethereum/evmc/pull/232)

### Changed

- The minimum supported GCC version is 6 (bumped from undocumented version 4.8).
  [#195](https://github.com/ethereum/evmc/pull/195)
- Go bindings improved by introduction of the `TxContext` struct.
  [#197](https://github.com/ethereum/evmc/pull/197)
- A lot improvements to the `evmc-vmtester` tool.
  [#221](https://github.com/ethereum/evmc/pull/221)
  [#234](https://github.com/ethereum/evmc/pull/234)
  [#238](https://github.com/ethereum/evmc/pull/238)
  [#241](https://github.com/ethereum/evmc/pull/241)
  [#242](https://github.com/ethereum/evmc/pull/242)
- [Cable] upgraded to version 0.2.17.
  [#251](https://github.com/ethereum/evmc/pull/251)

### Deprecated

- The `EVMC_CONSTANTINOPLE2` revision name is deprecated, replaced with `EVMC_PETERSBURG`.
  [#196](https://github.com/ethereum/evmc/pull/196)


## [6.1.1] - 2019-02-13

### Added

- Documentation of elements of `evmc_revision`.
  [#192](https://github.com/ethereum/evmc/pull/192)

### Fixed 

- Fixed compilation with GCC 5 because of the "deprecated" attribute applied
  to an enum element.
  [#190](https://github.com/ethereum/evmc/pull/190)


## [6.1.0] - 2019-01-24

### Added

- The **Istanbul** EVM revision has been added.
  [#174](https://github.com/ethereum/evmc/pull/174)
- The `is_zero()` C++ helper for basic data types has been added.
  [#182](https://github.com/ethereum/evmc/pull/182)
- Reserved the post-Constantinople EVM revision number.
  [#186](https://github.com/ethereum/evmc/pull/186)
- The C++ wrappers for VM and execution result objects have been added.
  [#187](https://github.com/ethereum/evmc/pull/187)

### Deprecated

- The `EVMC_LATEST_REVISION` name has been deprecated, replaced with `EVMC_MAX_REVISION`.
  [#184](https://github.com/ethereum/evmc/pull/184)


## [6.0.2] - 2019-01-16

### Fixed

- Add missing salt argument for CREATE2 in Host in Go bindings.
  [#179](https://github.com/ethereum/evmc/pull/179)


## [6.0.1] - 2018-11-10

### Fixed

- Integration of EVMC as a CMake subproject is easier because 
  Hunter is not loaded unless building tests (`EVMC_TESTING=ON`) is requested.
  [#169](https://github.com/ethereum/evmc/pull/169)


## [6.0.0] - 2018-10-24

### Added

- [EVMC Host implementation example](https://github.com/ethereum/evmc/blob/master/examples/example_host.cpp).
  [#116](https://github.com/ethereum/evmc/pull/116)
- Support for Constantinople SSTORE net gas metering.
  [#127](https://github.com/ethereum/evmc/pull/127)
- Support for Constantinople CREATE2 salt in Go bindings.
  [#133](https://github.com/ethereum/evmc/pull/133)
- A VM can now report its **capabilities** (i.e. EVM and/or ewasm).
  [#144](https://github.com/ethereum/evmc/pull/144)
- Introduction of the `evmc_bytes32` type.
  [#152](https://github.com/ethereum/evmc/pull/152)
- [EVMC Host implementation guide](https://ethereum.github.io/evmc/hostguide.html).
  [#159](https://github.com/ethereum/evmc/pull/159)
- [EVMC VM implementation guide](https://ethereum.github.io/evmc/vmguide.html).
  [#160](https://github.com/ethereum/evmc/pull/160)

### Changed

- EVMC loader symbol searching has been generalized.
  [#119](https://github.com/ethereum/evmc/pull/119)
- The `evmc_context_fn_table` renamed to `evmc_host_interface`.
  [#125](https://github.com/ethereum/evmc/pull/125)
- The `evmc_message` fields reordered.
  [#128](https://github.com/ethereum/evmc/pull/128)
- The `evmc_set_option()` now returns more information about the failure cause.
  [#136](https://github.com/ethereum/evmc/pull/136)
- In C the `bool` type is used instead of `int` for true/false flags.
  [#138](https://github.com/ethereum/evmc/pull/138)
  [#140](https://github.com/ethereum/evmc/pull/140)
- Simplification of signatures of Host methods.
  [#154](https://github.com/ethereum/evmc/pull/154)


## [5.2.0] - 2018-08-28

### Added

- Use also "evmc_create" function name for loading EVMC DLLs.
  [#81](https://github.com/ethereum/evmc/pull/81)

### Changed

- Documentation improvements, including documentation for the VM Tester.
  [#97](https://github.com/ethereum/evmc/pull/97)
  [#107](https://github.com/ethereum/evmc/pull/107)

### Fixed

- The `evmc.h` header compatibility with C++98 fixed.
  [#92](https://github.com/ethereum/evmc/pull/92)
- Compilation and build configuration fixes.
  [#93](https://github.com/ethereum/evmc/pull/93)
  [#103](https://github.com/ethereum/evmc/pull/103)


## [5.1.0] - 2018-08-23

### Added

- **Go language bindings** for EVMC.
  [#41](https://github.com/ethereum/evmc/pull/41)
- New error codes.
  [#56](https://github.com/ethereum/evmc/pull/56)
  [#62](https://github.com/ethereum/evmc/pull/62)
- More helper functions.
  [#67](https://github.com/ethereum/evmc/pull/67)
  [#68](https://github.com/ethereum/evmc/pull/68)
  [#70](https://github.com/ethereum/evmc/pull/70)

### Changed

- Documentation has been extended.
  [#58](https://github.com/ethereum/evmc/pull/58)
- Optional Result Storage helper module has been separated.
  [#59](https://github.com/ethereum/evmc/pull/59)
- [Cable] upgraded to 0.2.11.
  [#75](https://github.com/ethereum/evmc/pull/75)
- The license changed from MIT to **Apache 2.0**.
  [#77](https://github.com/ethereum/evmc/pull/77)

### Fixed

- Go bindings: Properly handle unknown error codes.
  [#72](https://github.com/ethereum/evmc/pull/72)


## [5.0.0] - 2018-08-10

### Added

- List of status codes extended and reordered.
  [#23](https://github.com/ethereum/evmc/pull/23)
  [#24](https://github.com/ethereum/evmc/pull/24)
- VM Tracing API.
  [#32](https://github.com/ethereum/evmc/pull/32)
- The support library with **metrics tables for EVM1 instructions**.
  [#33](https://github.com/ethereum/evmc/pull/33)
  [#34](https://github.com/ethereum/evmc/pull/34)
- Ability to create EVMC CMake package.
  [#35](https://github.com/ethereum/evmc/pull/35)
- The loader support library for VM dynamic loading.
  [#40](https://github.com/ethereum/evmc/pull/40)
- Constantinople: Support for `CREATE2` instruction.
  [#45](https://github.com/ethereum/evmc/pull/45)
- Constantinople: Support for `EXTCODEHASH` instruction.
  [#49](https://github.com/ethereum/evmc/pull/49)
- Constantinople: Storage status is reported back from `evmc_set_storage()`.
  [#52](https://github.com/ethereum/evmc/pull/52)


[10.1.0]: https://github.com/ethereum/evmc/releases/tag/v10.1.0
[10.0.0]: https://github.com/ethereum/evmc/releases/tag/v10.0.0
[9.0.0]: https://github.com/ethereum/evmc/releases/tag/v9.0.0
[8.0.0]: https://github.com/ethereum/evmc/releases/tag/v8.0.0
[7.5.0]: https://github.com/ethereum/evmc/releases/tag/v7.5.0
[7.4.0]: https://github.com/ethereum/evmc/releases/tag/v7.4.0
[7.3.0]: https://github.com/ethereum/evmc/releases/tag/v7.3.0
[7.2.0]: https://github.com/ethereum/evmc/releases/tag/v7.2.0
[7.1.0]: https://github.com/ethereum/evmc/releases/tag/v7.1.0
[7.0.0]: https://github.com/ethereum/evmc/releases/tag/v7.0.0
[6.3.1]: https://github.com/ethereum/evmc/releases/tag/v6.3.1
[6.3.0]: https://github.com/ethereum/evmc/releases/tag/v6.3.0
[6.2.2]: https://github.com/ethereum/evmc/releases/tag/v6.2.2
[6.2.1]: https://github.com/ethereum/evmc/releases/tag/v6.2.1
[6.2.0]: https://github.com/ethereum/evmc/releases/tag/v6.2.0
[6.1.1]: https://github.com/ethereum/evmc/releases/tag/v6.1.1
[6.1.0]: https://github.com/ethereum/evmc/releases/tag/v6.1.0
[6.0.2]: https://github.com/ethereum/evmc/releases/tag/v6.0.2
[6.0.1]: https://github.com/ethereum/evmc/releases/tag/v6.0.1
[6.0.0]: https://github.com/ethereum/evmc/releases/tag/v6.0.0
[5.2.0]: https://github.com/ethereum/evmc/releases/tag/v5.2.0
[5.1.0]: https://github.com/ethereum/evmc/releases/tag/v5.1.0
[5.0.0]: https://github.com/ethereum/evmc/releases/tag/v5.0.0

[Cable]: https://github.com/ethereum/cable
[Keep a Changelog]: https://keepachangelog.com/en/1.0.0/
[Semantic Versioning]: https://semver.org
