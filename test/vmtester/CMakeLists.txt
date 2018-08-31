# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

include(GNUInstallDirs)

hunter_add_package(CLI11)
find_package(CLI11)

add_executable(evmc-vmtester vmtester.hpp vmtester.cpp tests.cpp)
set_target_properties(evmc-vmtester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
target_link_libraries(evmc-vmtester PRIVATE evmc loader evmc-example-host GTest::gtest CLI11::CLI11)

install(TARGETS evmc-vmtester RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_test(NAME vmtester-help
    COMMAND evmc-vmtester --help
)
set_tests_properties(vmtester-help PROPERTIES
    PASS_REGULAR_EXPRESSION "Options:"
)

add_test(NAME vmtester-nonexistingvm
    COMMAND evmc-vmtester nonexistingvm
)
set_tests_properties(vmtester-nonexistingvm PROPERTIES
    WILL_FAIL TRUE
    FAIL_REGULAR_EXPRESSION "no such file"
)

add_test(NAME vmtester-novm
    COMMAND evmc-vmtester
)
set_tests_properties(vmtester-novm PROPERTIES
    WILL_FAIL TRUE
    FAIL_REGULAR_EXPRESSION "required but missing"
)
