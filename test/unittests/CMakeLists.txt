# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

add_executable(
    evmc-test
    test_instructions.cpp
)

target_link_libraries(evmc-test PRIVATE instructions GTest::gtest GTest::main)
