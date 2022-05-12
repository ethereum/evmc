# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

# This CMake script creates multiple additional targets to test the compilation of public headers
# with different C and C++ standards.

include(CheckCXXCompilerFlag)

set(standards c_std_99;c_std_11;cxx_std_17;cxx_std_20)

list(APPEND all_features ${CMAKE_CXX_COMPILE_FEATURES})
list(APPEND all_features ${CMAKE_C_COMPILE_FEATURES})

macro(create_compilation_test STANDARD)
    if(${STANDARD} MATCHES "^(c|cxx)_std_([0-9]+)$")
        set(lang ${CMAKE_MATCH_1})
        set(num ${CMAKE_MATCH_2})
    else()
        message(FATAL_ERROR "Unknown standard: ${STANDARD}")
    endif()

    if(${STANDARD} IN_LIST all_features)
        set(target test-compile-${STANDARD})
        add_library(${target} OBJECT compilation_test.${lang})
        target_compile_features(${target} PRIVATE ${STANDARD})
        target_include_directories(${target} PRIVATE ${EVMC_INCLUDE_DIR})
    else()
        message(STATUS "Compilation test SKIPPED: ${STANDARD}")
    endif()
endmacro()

foreach(standard ${standards})
    create_compilation_test(${standard})
endforeach()

check_cxx_compiler_flag(-fno-exceptions HAVE_NO_EXCEPTIONS)
if(HAVE_NO_EXCEPTIONS)
    add_library(test-compile-no-exceptions OBJECT compilation_test.cxx)
    target_compile_features(test-compile-no-exceptions PRIVATE cxx_std_17)
    target_compile_options(test-compile-no-exceptions PRIVATE -fno-exceptions)
    target_include_directories(test-compile-no-exceptions PRIVATE ${EVMC_INCLUDE_DIR})
endif()
