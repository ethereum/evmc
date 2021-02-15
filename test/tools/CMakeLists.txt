# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019-2020 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

function(add_evmc_tool_test NAME ARGUMENTS EXPECTED_OUTPUT)
    separate_arguments(ARGUMENTS)
    add_test(NAME ${PROJECT_NAME}/evmc-tool/${NAME} COMMAND evmc::tool ${ARGUMENTS})
    set_tests_properties(${PROJECT_NAME}/evmc-tool/${NAME} PROPERTIES PASS_REGULAR_EXPRESSION ${EXPECTED_OUTPUT})
endfunction()


add_evmc_tool_test(
    example1
    "--vm $<TARGET_FILE:evmc::example-vm> run 30600052596000f3 --gas 99"
    "Result: +success[\r\n]+Gas used: +6[\r\n]+Output: +0000000000000000000000000000000000000000000000000000000000000000[\r\n]"
)

add_evmc_tool_test(
    version
    "--version"
    "EVMC ${PROJECT_VERSION} \\($<TARGET_FILE:evmc::tool>\\)"
)

add_evmc_tool_test(
    version_vm
    "--vm $<TARGET_FILE:evmc::example-vm> --version"
    "example_vm ${PROJECT_VERSION} \\($<TARGET_FILE:evmc::example-vm>\\)[\r\n]EVMC ${PROJECT_VERSION} \\($<TARGET_FILE:evmc::tool>\\)"
)

add_evmc_tool_test(
    copy_input
    "--vm $<TARGET_FILE:evmc::example-vm> run 600035600052596000f3 --input aabbccdd"
    "Result: +success[\r\n]+Gas used: +7[\r\n]+Output: +aabbccdd00000000000000000000000000000000000000000000000000000000[\r\n]"
)

get_property(TOOLS_TESTS DIRECTORY PROPERTY TESTS)
set_tests_properties(${TOOLS_TESTS} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/tools-%p.profraw)
