cmake_minimum_required(VERSION 3.5)

if(TARGET evmc)
    # The evmc library has been already created (probably by other submodule).
    return()
endif()

project(evmc)

add_library(evmc INTERFACE)
target_include_directories(evmc INTERFACE include)

option(EVMC_BUILD_EXAMPLES "Build EVMC examples" ON)
if(EVMC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
