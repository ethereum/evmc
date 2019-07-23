# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.


# Adds a CMake test to check the given EVMC VM implementation with the evmc-vmtester tool.
#
# evmc_add_vm_test(NAME <test_name> TARGET <vm>)
# - NAME argument specifies the name of the added test,
# - TARGET argument specifies the CMake target being a shared library with EVMC VM implementation.
function(evmc_add_vm_test)
    if(NOT TARGET evmc::evmc-vmtester)
        message(FATAL_ERROR "The evmc-vmtester has not been installed with this EVMC package")
    endif()

    cmake_parse_arguments("" "" NAME;TARGET "" ${ARGN})
    add_test(NAME ${_NAME} COMMAND $<TARGET_FILE:evmc::evmc-vmtester> $<TARGET_FILE:${_TARGET}>)
endfunction()
