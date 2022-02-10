# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(
    instructions STATIC
    ${EVMC_INCLUDE_DIR}/evmc/instructions.h
    instruction_metrics.c
    instruction_names.c
)

add_library(evmc::instructions ALIAS instructions)
set_target_properties(instructions PROPERTIES
    OUTPUT_NAME evmc-instructions
    POSITION_INDEPENDENT_CODE TRUE
)
target_include_directories(instructions PUBLIC
    $<BUILD_INTERFACE:${EVMC_INCLUDE_DIR}>$<INSTALL_INTERFACE:include>
)

if(EVMC_INSTALL)
    install(TARGETS instructions EXPORT evmcTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
