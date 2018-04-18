# EVMC -- Ethereum Client-VM Connector API
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

cmake_minimum_required(VERSION 3.5)

if(TARGET evmc)
    # The evmc library has been already created (probably by other submodule).
    return()
endif()

include(cmake/cable/bootstrap.cmake)
include(CableCompilerSettings)
include(HunterGate)
include(GNUInstallDirs)
include(defaults/HunterCacheServers)

# Setup Hunter.
# Hunter is going to be initialized only if building with tests,
# where it is needed to get dependencies.
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.20.37.tar.gz"
    SHA1 "51886d10428c924cc21756abc17623bcf4986386"
)

project(evmc)
set(PROJECT_VERSION "0.1.0.dev0")

cable_configure_compiler()

add_library(evmc INTERFACE)
target_include_directories(evmc INTERFACE include)

install(DIRECTORY include/evmc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

option(EVMC_BUILD_TESTS "Build EVMC tests and test tools" OFF)
if(EVMC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

option(EVMC_BUILD_EXAMPLES "Build EVMC examples" OFF)
if(EVMC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
