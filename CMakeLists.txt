# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

cmake_minimum_required(VERSION 3.5)

if(TARGET evmc)
    # The evmc library has been already created (probably by other submodule).
    return()
endif()

option(EVMC_BUILD_TESTS "Build EVMC tests and test tools" OFF)
option(EVMC_BUILD_EXAMPLES "Build EVMC examples" OFF)
option(HUNTER_ENABLED "Enable Hunter package manager support" "${EVMC_BUILD_TESTS}")

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

cable_configure_compiler(NO_STACK_PROTECTION)

add_library(evmc INTERFACE)
add_library(evmc::evmc ALIAS evmc)
target_include_directories(evmc INTERFACE include)

install(DIRECTORY include/evmc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(EVMC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(EVMC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


if(WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()
string(TOLOWER ${CMAKE_SYSTEM_NAME} system_name)
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION}-${system_name})
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
include(CPack)
