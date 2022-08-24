# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(EVMC)

set(prefix ${PROJECT_NAME}/vmtester)

evmc_add_vm_test(NAME ${prefix}/examplevm TARGET example-vm)
evmc_add_vm_test(NAME ${prefix}/example_precompiles_vm TARGET example-precompiles-vm)

add_test(NAME ${prefix}/help COMMAND evmc::evmc-vmtester --version --help)
set_tests_properties(${prefix}/help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

add_test(NAME ${prefix}/nonexistingvm COMMAND evmc::evmc-vmtester nonexistingvm)
set_tests_properties(${prefix}/nonexistingvm PROPERTIES PASS_REGULAR_EXPRESSION "[Cc]annot open|no such file")

add_test(NAME ${prefix}/noarg COMMAND evmc::evmc-vmtester)
set_tests_properties(${prefix}/noarg PROPERTIES PASS_REGULAR_EXPRESSION "is required")

add_test(NAME ${prefix}/too-many-args COMMAND evmc::evmc-vmtester a b)
set_tests_properties(${prefix}/too-many-args PROPERTIES PASS_REGULAR_EXPRESSION "Unexpected")

add_test(NAME ${prefix}/version COMMAND evmc::evmc-vmtester --version)
set_tests_properties(${prefix}/version PROPERTIES PASS_REGULAR_EXPRESSION ${PROJECT_VERSION})

add_test(NAME ${prefix}/unknown-option COMMAND evmc::evmc-vmtester --verbose)
set_tests_properties(${prefix}/unknown-option PROPERTIES PASS_REGULAR_EXPRESSION "Unknown")

add_test(NAME ${prefix}/option-long-prefix COMMAND evmc::evmc-vmtester ---)
set_tests_properties(${prefix}/option-long-prefix PROPERTIES PASS_REGULAR_EXPRESSION "Unknown")

get_property(vmtester_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${vmtester_tests} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/vmtester-%m-%p.profraw)
