# EVMC VM Tester {#vmtester}

The EVMC project contains a EVMC-compatibility testing tool for VM implementations.

The tool is called `evmc-vmtester` and to include it in the EVMC build
add `-DEVMC_TESTING=ON` CMake option to the project configuration step.

Usage is simple as

```sh
evmc-vmtester [vm]
```

where `[vm]` is a path to a shared library with VM implementation.

For more information check `evmc-vmtester --help`.
