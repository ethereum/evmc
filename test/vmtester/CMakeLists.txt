# EVMC -- Ethereum Client-VM Connector API
# Copyright 2018 Pawel Bylica.
# Licensed under the MIT License. See the LICENSE file.

hunter_add_package(Boost COMPONENTS filesystem system)
find_package(Boost CONFIG REQUIRED filesystem system)

add_executable(vmtester vmtester.cpp)
target_link_libraries(vmtester PRIVATE evmc Boost::filesystem Boost::system ${CMAKE_DL_LIBS})
