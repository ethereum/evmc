# EVMC: Ethereum Client-VM Connector API.
# Copyright 2016-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

cmake_minimum_required(VERSION 3.5)

if(TARGET evmc)
    # The evmc library has been already created (probably by other submodule).
    return()
endif()

include(cmake/cable/bootstrap.cmake)
include(CableBuildType)
include(CableCompilerSettings)
include(CMakeDependentOption)
include(CablePackage)
include(CableToolchains)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(EVMC_INSTALL "Enable EVMC installation (e.g. make install)" ON)

option(EVMC_TESTING "Build everything (libraries, tools, examples, internal tests)" OFF)

cmake_dependent_option(EVMC_TOOLS "Build EVMC tools" OFF
    "NOT EVMC_TESTING" ON)

cmake_dependent_option(EVMC_EXAMPLES "Build EVMC examples" OFF
    "NOT EVMC_TESTING" ON)

option(HUNTER_ENABLED "Enable Hunter package manager support" ${EVMC_TOOLS})

if(HUNTER_ENABLED)
    set(HUNTER_CONFIGURATION_TYPES Release CACHE STRING "Build type of Hunter packages")
    include(HunterGate)
    include(Hunter/init)
    include(defaults/HunterCacheServers)
endif()

cable_configure_toolchain(DEFAULT cxx11-pic)
cable_set_build_type(DEFAULT Release CONFIGURATION_TYPES Debug Release)

project(evmc)
set(PROJECT_VERSION 7.1.0)

cable_configure_compiler(NO_STACK_PROTECTION)

set(include_dir ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(lib)

if(EVMC_TOOLS)
    add_subdirectory(tools)
endif()

if(EVMC_EXAMPLES)
    add_subdirectory(examples)
endif()

if(EVMC_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

write_basic_package_version_file(evmcConfigVersion.cmake COMPATIBILITY ExactVersion)

configure_package_config_file(
    cmake/Config.cmake.in
    evmcConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/evmc
)

if(EVMC_INSTALL)
    install(DIRECTORY include/evmc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(
        EXPORT evmcTargets
        NAMESPACE evmc::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/evmc
    )
    install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/evmcConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/evmcConfigVersion.cmake
        cmake/EVMC.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/evmc
    )
endif()

set(
    CPACK_SOURCE_IGNORE_FILES
    /\\\\.git/
    /\\\\.idea/
    /build/
    /cmake-build-.*/
    /bindings/
    /Cargo.lock
    /Cargo.toml
    /target/
)
cable_add_archive_package()
