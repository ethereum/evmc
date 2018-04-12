# EVMC -- Ethereum Client-VM Connector API
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

cmake_minimum_required(VERSION 3.5)

if(TARGET evmc)
    # The evmc library has been already created (probably by other submodule).
    return()
endif()

include(cmake/cable/bootstrap.cmake)
include(HunterGate)
include(defaults/HunterCacheServers)

# Setup Hunter.
# Hunter is going to be initialized only if building with tests,
# where it is needed to get dependencies.
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.20.37.tar.gz"
    SHA1 "51886d10428c924cc21756abc17623bcf4986386"
)

project(evmc)

add_library(evmc INTERFACE)
target_include_directories(evmc INTERFACE include)

option(EVMC_BUILD_EXAMPLES "Build EVMC examples" ON)
if(EVMC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
