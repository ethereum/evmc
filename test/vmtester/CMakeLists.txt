# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(GNUInstallDirs)

add_executable(evmc-vmtester vmtester.hpp vmtester.cpp tests.cpp)
set_target_properties(evmc-vmtester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
target_link_libraries(evmc-vmtester PRIVATE evmc loader evmc-example-host GTest::gtest)
set_source_files_properties(vmtester.cpp PROPERTIES COMPILE_DEFINITIONS PROJECT_VERSION="${PROJECT_VERSION}")


install(TARGETS evmc-vmtester RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_test(NAME vmtester/examplevm COMMAND evmc-vmtester $<TARGET_FILE:evmc-example-vm>)
add_test(NAME vmtester/example_precompiles_vm COMMAND evmc-vmtester $<TARGET_FILE:evmc-example-precompiles-vm>)

add_test(NAME vmtester/help COMMAND evmc-vmtester --version --help)
set_tests_properties(vmtester/help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

add_test(NAME vmtester/nonexistingvm COMMAND evmc-vmtester nonexistingvm)
set_tests_properties(vmtester/nonexistingvm PROPERTIES PASS_REGULAR_EXPRESSION "[Cc]annot open")

add_test(NAME vmtester/noarg COMMAND evmc-vmtester)
set_tests_properties(vmtester/noarg PROPERTIES PASS_REGULAR_EXPRESSION "is required")

add_test(NAME vmtester/too-many-args COMMAND evmc-vmtester a b)
set_tests_properties(vmtester/too-many-args PROPERTIES PASS_REGULAR_EXPRESSION "Unexpected")

add_test(NAME vmtester/version COMMAND evmc-vmtester --version)
set_tests_properties(vmtester/version PROPERTIES PASS_REGULAR_EXPRESSION ${PROJECT_VERSION})

add_test(NAME vmtester/unknown-option COMMAND evmc-vmtester --verbose)
set_tests_properties(vmtester/unknown-option PROPERTIES PASS_REGULAR_EXPRESSION "Unknown")

add_test(NAME vmtester/option-long-prefix COMMAND evmc-vmtester ---)
set_tests_properties(vmtester/option-long-prefix PROPERTIES PASS_REGULAR_EXPRESSION "Unknown")
