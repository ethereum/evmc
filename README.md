# EVMC

[![chat: on gitter][gitter badge]][Gitter]
[![readme style: standard][readme style standard badge]][standard readme]

> Ethereum Client-VM Connector API

The EVMC is the low-level ABI between Ethereum Virtual Machines (EVMs) and 
Ethereum Clients. On the EVM-side it supports classic EVM1 and [eWASM].
On the Client-side it defines the interface for EVM implementations 
to access Ethereum environment and state.

## Usage

Please visit the [documentation].

## Related projects

### EVMs

- [evmjit]
- [Hera]
- "[interpreter]"

### Clients

- [cpp-ethereum]
- [nim-evmjit]
- [go-ethereum] (in progress)
- [pyevm] (in progress)

## Contribute

[![chat: on gitter][gitter badge]][Gitter]

Talk with us on the [EVMC Gitter chat][Gitter].

## Maintainers

- Alex Beregszaszi [@axic]
- Paweł Bylica [@chfast]

## License

Licensed under the [MIT License](LICENSE.md).


[@axic]: https://github.com/axic
[@chfast]: https://github.com/chfast
[documentation]: https://ethereum.github.io/evmc
[eWASM]: https://github.com/ewasm/design#ethereum-flavored-webassembly-ewasm-design
[evmjit]: https://github.com/ethereum/evmjit
[Hera]: https://github.com/ewasm/hera
[Gitter]: https://gitter.im/ethereum/evmc
[interpreter]: https://github.com/ethereum/cpp-ethereum/blob/develop/libevm/VM.cpp
[cpp-ethereum]: https://github.com/ethereum/cpp-ethereum
[nim-evmjit]: https://github.com/status-im/nim-evmjit
[go-ethereum]: https://github.com/ethereum/go-ethereum
[pyevm]: https://github.com/ethereum/cpp-ethereum
[standard readme]: https://github.com/RichardLitt/standard-readme

[gitter badge]: https://img.shields.io/gitter/room/ethereum/evmc.svg?style=flat-square
[readme style standard badge]: https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square