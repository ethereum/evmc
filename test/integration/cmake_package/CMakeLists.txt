# EVMC: Ethereum Client-VM Connector API.
# Copyright 2019 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

set(install_dir ${CMAKE_CURRENT_BINARY_DIR}/install)
set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/build)
set(use_evmc_build_dir ${CMAKE_CURRENT_BINARY_DIR}/use_evmc)
set(use_instructions_build_dir ${CMAKE_CURRENT_BINARY_DIR}/use_instructions)

file(MAKE_DIRECTORY ${build_dir} ${use_evmc_build_dir} ${use_instructions_build_dir})

add_test(
    NAME cmake_package/configure
    COMMAND ${CMAKE_COMMAND} ${PROJECT_SOURCE_DIR} -DCMAKE_INSTALL_PREFIX=${install_dir}
    WORKING_DIRECTORY ${build_dir}
)

add_test(
    NAME cmake_package/install
    COMMAND ${CMAKE_COMMAND} --build ${build_dir} --target install
)
set_tests_properties(cmake_package/install PROPERTIES DEPENDS cmake_package/configure)


add_test(
    NAME cmake_package/use_evmc/configure
    COMMAND ${CMAKE_COMMAND} ${PROJECT_SOURCE_DIR}/examples/use_evmc_in_cmake -DCMAKE_PREFIX_PATH=${install_dir}
    WORKING_DIRECTORY ${use_evmc_build_dir}
)
set_tests_properties(cmake_package/use_evmc/configure PROPERTIES DEPENDS cmake_package/install)

add_test(
    NAME cmake_package/use_evmc/build
    COMMAND ${CMAKE_COMMAND} --build ${use_evmc_build_dir}
)
set_tests_properties(cmake_package/use_evmc/build PROPERTIES DEPENDS cmake_package/use_evmc/configure)


add_test(
    NAME cmake_package/use_instructions/configure
    COMMAND ${CMAKE_COMMAND} ${PROJECT_SOURCE_DIR}/examples/use_instructions_in_cmake -DCMAKE_PREFIX_PATH=${install_dir}
    WORKING_DIRECTORY ${use_instructions_build_dir}
)
set_tests_properties(cmake_package/use_instructions/configure PROPERTIES DEPENDS cmake_package/install)

add_test(
    NAME cmake_package/use_instructions/build
    COMMAND ${CMAKE_COMMAND} --build ${use_instructions_build_dir}
)
set_tests_properties(cmake_package/use_instructions/build PROPERTIES DEPENDS cmake_package/use_instructions/configure)
