# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

# This CMake script creates multiple additional targets to test the compilation of public headers
# with different C and C++ standards.

set(standards c_std_99;c_std_11;cxx_std_11;cxx_std_14)
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.8.1)
    list(APPEND standards cxx_std_17)
endif()

list(APPEND all_features ${CMAKE_CXX_COMPILE_FEATURES})
list(APPEND all_features ${CMAKE_C_COMPILE_FEATURES})

macro(create_compilation_test STANDARD)
    if (${STANDARD} MATCHES "^(c|cxx)_std_([0-9]+)$")
        set(lang ${CMAKE_MATCH_1})
        set(num ${CMAKE_MATCH_2})
    else()
        message(FATAL_ERROR "Unknown standard: ${STANDARD}")
    endif()

    if(${STANDARD} IN_LIST all_features)
        set(target test-compile-${STANDARD})
        add_library(${target} OBJECT compilation_test.${lang})
        target_compile_features(${target} PRIVATE ${STANDARD})
        target_include_directories(${target} PRIVATE ${include_dir})
    else()
        message(STATUS "Compilation test SKIPPED: ${STANDARD}")
    endif()
endmacro()

foreach(standard ${standards})
    create_compilation_test(${standard})
endforeach()
