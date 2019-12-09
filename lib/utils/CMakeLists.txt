# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

add_library(
    utils STATIC
    ${include_dir}/evmc/utils.hpp
    utils.cpp
)

add_library(evmc::utils ALIAS utils)
target_compile_features(utils PUBLIC cxx_std_17)
target_include_directories(utils PUBLIC $<BUILD_INTERFACE:${include_dir}>$<INSTALL_INTERFACE:include>)
set_target_properties(loader PROPERTIES
    OUTPUT_NAME evmc-utils
    POSITION_INDEPENDENT_CODE TRUE
)

if(EVMC_INSTALL)
    install(TARGETS utils EXPORT evmcTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
