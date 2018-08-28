# EVMC: Ethereum Client-VM Connector API
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

include(GNUInstallDirs)

add_library(evmc-examplevm examplevm.c)
target_link_libraries(evmc-examplevm PRIVATE evmc)
set_target_properties(
    evmc-examplevm PROPERTIES
    DEBUG_POSTFIX ""
    RUNTIME_OUTPUT_DIRECTORY ..
    LIBRARY_OUTPUT_DIRECTORY ..
)
set_source_files_properties(examplevm.c PROPERTIES COMPILE_DEFINITIONS PROJECT_VERSION=${PROJECT_VERSION})

install(
    TARGETS evmc-examplevm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
