# EVMC

[![chat: on gitter][gitter badge]][Gitter]
[![readme style: standard][readme style standard badge]][standard readme]

> Ethereum Client-VM Connector API

The EVMC is the low-level ABI between Ethereum Virtual Machines (EVMs) and
Ethereum Clients. On the EVM side it supports classic EVM1 and [ewasm].
On the Client-side it defines the interface for EVM implementations
to access Ethereum environment and state.


## Usage

### Documentation

Please visit the [documentation].

### Languages support

| Language                      | Supported Versions    | Supported Compilers
| ----------------------------- | --------------------- | ------------------------------
| **C**                         | C99, C11              | GCC 6+, clang 3.8+, MSVC 2015+
| **C++**                       | C++11, C++14, C++17   | GCC 6+, clang 3.8+, MSVC 2015+
| **Go** _(bindings)_           | 1.9 - 1.12            |
| **Rust** _(bindings)_[¹](#n1) | 2018 edition          | 1.37.0 and newer

<b id="n1">1</b>. Rust support is limited and not complete yet, but it is mostly functional already. Breaking changes are possible at this stage.

### Testing tools

* **evmc-vmtester** ([test/vmtester]) — can test any EVM implementation for compatibility with EVMC.
* **evm-test** ([evmone → test/unittests]) — allows running the collection of [evmone]'s unit tests on any EVMC-compatible EVM implementation.
* **evmone-fuzzer** ([evmone → test/fuzzer]) — differential fuzzer for EVMC-compatible EVM implementations. 


## Related projects

### EVMs

- [aleth-interpreter]
- [evmjit]
- [evmone]
- [Hera]
- [Hera.rs]
- [Daytona]
- [eip1962-evmc] (EIP-2003 style precompile)

### Clients

- [aleth]
- [nim-evmc]
- [go-ethereum] (in progress)
- [pyevm] (in progress)
- [pyethereum] (abandoned)
- [Solidity] (for integration testing)


## Maintainers

- Alex Beregszaszi [@axic]
- Paweł Bylica [@chfast]

See also the list of [EVMC Authors](AUTHORS.md).

## Contributing

[![chat: on gitter][gitter badge]][Gitter]

Talk with us on the [EVMC Gitter chat][Gitter].

## License

Licensed under the [MIT License](LICENSE).


## Internal

### Making new release

1. Update [CHANGELOG.md](CHANGELOG.md), put the release date, update release link.
2. `git add CHANGELOG.md`.
3. Tag new release: `bumpversion --allow-dirty prerel`.
4. Prepare CHANGELOG for next release: add unreleased section and link.
5. `git add CHANGELOG.md`.
6. Start new release series: `bumpversion --allow-dirty --no-tag minor`.


[@axic]: https://github.com/axic
[@chfast]: https://github.com/chfast
[documentation]: https://ethereum.github.io/evmc
[ewasm]: https://github.com/ewasm/design
[evmjit]: https://github.com/ethereum/evmjit
[evmone]: https://github.com/ethereum/evmone
[evmone → test/fuzzer]: https://github.com/ethereum/evmone/tree/master/test/fuzzer
[evmone → test/unittests]: https://github.com/ethereum/evmone/tree/master/test/unittests
[Hera]: https://github.com/ewasm/hera
[Hera.rs]: https://github.com/ewasm/hera.rs
[Daytona]: https://github.com/axic/daytona
[eip1962-evmc]: https://github.com/axic/eip1962-evmc
[Gitter]: https://gitter.im/ethereum/evmc
[aleth-interpreter]: https://github.com/ethereum/aleth/tree/master/libaleth-interpreter
[aleth]: https://github.com/ethereum/aleth
[Solidity]: https://github.com/ethereum/solidity
[nim-evmc]: https://github.com/status-im/nim-evmc
[go-ethereum]: https://github.com/ethereum/go-ethereum/pull/17954
[pyevm]: https://github.com/ethereum/py-evm
[pyethereum]: https://github.com/ethereum/pyethereum/pull/406
[standard readme]: https://github.com/RichardLitt/standard-readme
[test/vmtester]: https://github.com/ethereum/evmc/tree/master/test/vmtester

[gitter badge]: https://img.shields.io/gitter/room/ethereum/evmc.svg
[readme style standard badge]: https://img.shields.io/badge/readme%20style-standard-brightgreen.svg
