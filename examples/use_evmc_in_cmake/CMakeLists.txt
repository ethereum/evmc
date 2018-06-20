# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

# This example shows how to use evmc INTERFACE library from evmc CMake package.

cmake_minimum_required(VERSION 3.5)

project(use_evmc_in_cmake)

find_package(evmc CONFIG REQUIRED)

add_executable(use_evmc_in_cmake use_evmc_in_cmake.c)
target_link_libraries(use_evmc_in_cmake PRIVATE evmc::evmc)
