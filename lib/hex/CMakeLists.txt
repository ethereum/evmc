# EVMC: Ethereum Client-VM Connector API.
# Copyright 2021 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(
    hex STATIC
    ${EVMC_INCLUDE_DIR}/evmc/hex.hpp
    hex.cpp
)

add_library(evmc::hex ALIAS hex)
target_compile_features(hex PUBLIC cxx_std_17)
target_include_directories(hex PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>$<INSTALL_INTERFACE:include>)
set_target_properties(hex PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

if(EVMC_INSTALL)
    install(TARGETS hex EXPORT evmcTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
