# Changelog

## [5.2.0-dev] - unreleased

## [5.1.0] - 2018-08-23

- Feature: [[#41](https://github.com/ethereum/evmc/pull/41)]
  Go language bindings for EVMC.
- Feature: [[#56](https://github.com/ethereum/evmc/pull/56), [#62](https://github.com/ethereum/evmc/pull/62)]
  New error codes.
- Feature: [[#67](https://github.com/ethereum/evmc/pull/67), [#68](https://github.com/ethereum/evmc/pull/68), [#70](https://github.com/ethereum/evmc/pull/70)]
  More helper functions.
- Fix: [[#72](https://github.com/ethereum/evmc/pull/72)]
  Go bindings: Properly handle unknown error codes.
- Improved: [[#58](https://github.com/ethereum/evmc/pull/58)]
  Documentation has been extended.
- Improved: [[#59](https://github.com/ethereum/evmc/pull/59)]
  Optional Result Storage helper module has been separated.
- Improved: [[#75](https://github.com/ethereum/evmc/pull/75)]
  Cable upgraded to 0.2.11.
- Improved: [[#77](https://github.com/ethereum/evmc/pull/77)]
  The license changed from MIT to Apache 2.0.

## [5.0.0] - 2018-08-10

- Feature: [[#23](https://github.com/ethereum/evmc/pull/23), [#24](https://github.com/ethereum/evmc/pull/24)]
  List of status codes extended and reordered.
- Feature: [[#32](https://github.com/ethereum/evmc/pull/32)]
  VM Tracing API.
- Feature: [[#33](https://github.com/ethereum/evmc/pull/33), [#34](https://github.com/ethereum/evmc/pull/34)]
  The support library with metrics tables for EVM1 instructions.
- Feature: [[#35](https://github.com/ethereum/evmc/pull/35)]
  Ability to create EVMC CMake package.
- Feature: [[#40](https://github.com/ethereum/evmc/pull/40)]
  The loader support library for VM dynamic loading.
- Feature: [[#45](https://github.com/ethereum/evmc/pull/45)]
  Constantinople: Support for `CREATE2` instruction.
- Feature: [[#49](https://github.com/ethereum/evmc/pull/49)]
  Constantinople: Support for `EXTCODEHASH` instruction.
- Feature: [[#52](https://github.com/ethereum/evmc/pull/52)]
  Constantinople: Storage status is reported back from `evmc_set_storage()`.


[5.2.0-dev]: https://github.com/ethereum/evmc/compare/v5.1.0...HEAD
[5.1.0]: https://github.com/ethereum/evmc/releases/tag/v5.1.0
[5.0.0]: https://github.com/ethereum/evmc/releases/tag/v5.0.0