# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(EVMC)
include(GNUInstallDirs)

add_executable(evmc-vmtester vmtester.hpp vmtester.cpp tests.cpp)
set_target_properties(evmc-vmtester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
target_link_libraries(evmc-vmtester PRIVATE evmc loader evmc-example-host GTest::gtest)
set_source_files_properties(vmtester.cpp PROPERTIES COMPILE_DEFINITIONS PROJECT_VERSION="${PROJECT_VERSION}")
add_executable(evmc::evmc-vmtester ALIAS evmc-vmtester)


if(EVMC_INSTALL)
    install(TARGETS evmc-vmtester EXPORT evmcTargets RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

set(prefix ${PROJECT_NAME}/vmtester)

evmc_add_vm_test(NAME ${prefix}/examplevm TARGET evmc-example-vm)
evmc_add_vm_test(NAME ${prefix}/example_precompiles_vm TARGET evmc-example-precompiles-vm)

add_test(NAME ${prefix}/help COMMAND evmc-vmtester --version --help)
set_tests_properties(${prefix}/help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

add_test(NAME ${prefix}/nonexistingvm COMMAND evmc-vmtester nonexistingvm)
set_tests_properties(${prefix}/nonexistingvm PROPERTIES PASS_REGULAR_EXPRESSION "[Cc]annot open")

add_test(NAME ${prefix}/noarg COMMAND evmc-vmtester)
set_tests_properties(${prefix}/noarg PROPERTIES PASS_REGULAR_EXPRESSION "is required")

add_test(NAME ${prefix}/too-many-args COMMAND evmc-vmtester a b)
set_tests_properties(${prefix}/too-many-args PROPERTIES PASS_REGULAR_EXPRESSION "Unexpected")

add_test(NAME ${prefix}/version COMMAND evmc-vmtester --version)
set_tests_properties(${prefix}/version PROPERTIES PASS_REGULAR_EXPRESSION ${PROJECT_VERSION})

add_test(NAME ${prefix}/unknown-option COMMAND evmc-vmtester --verbose)
set_tests_properties(${prefix}/unknown-option PROPERTIES PASS_REGULAR_EXPRESSION "Unknown")

add_test(NAME ${prefix}/option-long-prefix COMMAND evmc-vmtester ---)
set_tests_properties(${prefix}/option-long-prefix PROPERTIES PASS_REGULAR_EXPRESSION "Unknown")
