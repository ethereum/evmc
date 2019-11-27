# EVMC: Ethereum Client-VM Connector API.
# Copyright 2018-2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(GNUInstallDirs)

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

# Disable support for std::tr1::tuple in GTest. This causes problems in Visual Studio 2015.
set_target_properties(GTest::gtest PROPERTIES INTERFACE_COMPILE_DEFINITIONS GTEST_HAS_TR1_TUPLE=0)

add_executable(evmc-vmtester vmtester.hpp vmtester.cpp tests.cpp)
set_target_properties(evmc-vmtester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
target_link_libraries(evmc-vmtester PRIVATE evmc::loader evmc::mocked_host GTest::gtest)
set_source_files_properties(vmtester.cpp PROPERTIES COMPILE_DEFINITIONS PROJECT_VERSION="${PROJECT_VERSION}")
add_executable(evmc::evmc-vmtester ALIAS evmc-vmtester)

if(EVMC_INSTALL)
    install(TARGETS evmc-vmtester EXPORT evmcTargets RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
