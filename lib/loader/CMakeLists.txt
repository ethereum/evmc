# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

add_library(
    loader STATIC
    ${include_dir}/evmc/loader.h
    loader.c
)

add_library(evmc::loader ALIAS loader)
set_target_properties(loader PROPERTIES OUTPUT_NAME evmc-loader)
target_include_directories(loader PUBLIC $<BUILD_INTERFACE:${include_dir}>$<INSTALL_INTERFACE:include>)
if(CMAKE_DL_LIBS)
    target_link_libraries(loader INTERFACE ${CMAKE_DL_LIBS})
endif()

install(TARGETS loader EXPORT evmcTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
