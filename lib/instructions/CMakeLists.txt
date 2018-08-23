# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

add_library(
    instructions STATIC
    ${include_dir}/evmc/instructions.h
    instruction_metrics.c
    instruction_names.c
)

add_library(evmc::instructions ALIAS instructions)
set_target_properties(instructions PROPERTIES OUTPUT_NAME evmc-instructions)
target_include_directories(instructions PUBLIC $<BUILD_INTERFACE:${include_dir}>$<INSTALL_INTERFACE:include>)
if(BUILD_SHARED_LIBS)
    set_target_properties(instructions PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif()

install(TARGETS instructions EXPORT evmcTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
