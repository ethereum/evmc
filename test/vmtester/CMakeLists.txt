# EVMC -- Ethereum Client-VM Connector API
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

hunter_add_package(Boost COMPONENTS program_options filesystem system)
find_package(Boost CONFIG REQUIRED program_options filesystem system)

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

add_executable(vmtester vmtester.hpp vmtester.cpp tests.cpp)

target_link_libraries(
    vmtester
    PRIVATE
    evmc
    GTest::gtest
    Boost::program_options
    Boost::filesystem
    Boost::system
    ${CMAKE_DL_LIBS}
)
