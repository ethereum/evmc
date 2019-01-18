# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

# This CMake script creates multiple additional targets to test the compilation of public headers
# with different C and C++ standards.

macro(create_compilation_test STANDARD)
    if (${STANDARD} MATCHES "^(C|CXX)([0-9]+)$")
        set(lang ${CMAKE_MATCH_1})
        set(num ${CMAKE_MATCH_2})
    else()
        message(FATAL_ERROR "Unknown language standard: ${STANDARD}")
    endif()

    if (lang STREQUAL CXX)
        set(ext cpp)
    else()
        set(ext c)
    endif()

    string(TOLOWER ${STANDARD} standard)
    set(target test-compile-${standard})

    add_library(${target} OBJECT compilation_test.${ext})
    target_include_directories(${target} PRIVATE ${include_dir})
    set_target_properties(${target} PROPERTIES ${lang}_STANDARD ${num} ${lang}_EXTENSIONS OFF)
endmacro()

foreach(standard C90;C99;C11;CXX11;CXX14;CXX17)
    create_compilation_test(${standard})
endforeach()
