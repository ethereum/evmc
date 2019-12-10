# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

set(prefix ${PROJECT_NAME}/evmc-run)

add_test(
    NAME ${prefix}/example1
    COMMAND evmc::tool run --vm $<TARGET_FILE:evmc::example-vm> 30600052596000f3 --gas 99
)
set_tests_properties(
    ${prefix}/example1 PROPERTIES PASS_REGULAR_EXPRESSION
    "Result: +success[\r\n]+Gas used: +99[\r\n]+Output: +0000000000000000000000000000000000000000[\r\n]"
)

get_property(tools_tests DIRECTORY PROPERTY TESTS)
set_tests_properties(${tools_tests} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/tools-%p.profraw)
