# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019-2020 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

set(PREFIX ${PROJECT_NAME}/evmc-run)

add_test(
    NAME ${PREFIX}/example1
    COMMAND evmc::tool run --vm $<TARGET_FILE:evmc::example-vm> 30600052596000f3 --gas 99
)
set_tests_properties(
    ${PREFIX}/example1 PROPERTIES PASS_REGULAR_EXPRESSION
    "Result: +success[\r\n]+Gas used: +6[\r\n]+Output: +0000000000000000000000000000000000000000000000000000000000000000[\r\n]"
)


add_test(
    NAME ${PREFIX}/copy_input
    COMMAND evmc::tool run --vm $<TARGET_FILE:evmc::example-vm> 600035600052596000f3 --input aabbccdd
)
set_tests_properties(
    ${PREFIX}/copy_input PROPERTIES PASS_REGULAR_EXPRESSION
    "Result: +success[\r\n]+Gas used: +7[\r\n]+Output: +aabbccdd00000000000000000000000000000000000000000000000000000000[\r\n]"
)

get_property(TOOLS_TESTS DIRECTORY PROPERTY TESTS)
set_tests_properties(${TOOLS_TESTS} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/tools-%p.profraw)
