# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 The EVMC Authors.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

include(GNUInstallDirs)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()
set(CMAKE_DEBUG_POSTFIX "")


add_library(evmc-example-host STATIC example_host.cpp)
target_link_libraries(evmc-example-host PRIVATE evmc)


add_library(evmc-examplevm examplevm.c)
target_link_libraries(evmc-examplevm PRIVATE evmc)
set_source_files_properties(examplevm.c PROPERTIES COMPILE_DEFINITIONS PROJECT_VERSION=${PROJECT_VERSION})


add_executable(evmc-example example.c)
target_link_libraries(evmc-example PRIVATE evmc-example-host evmc-examplevm evmc)


install(TARGETS evmc-examplevm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)