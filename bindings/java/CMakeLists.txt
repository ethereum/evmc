# EVMC: Ethereum Client-VM Connector API.
# Copyright 2020 The EVMC Authors.
# Licensed under the Apache License, Version 2.0.

include(UseJava)

find_package(Java REQUIRED COMPONENTS Development)
find_package(JNI REQUIRED)

add_jar(evmc-jar
    OUTPUT_NAME evmc-java
    VERSION ${PROJECT_VERSION}
    SOURCES
    java/src/main/java/org/ethereum/evmc/EvmcLoaderException.java
    java/src/main/java/org/ethereum/evmc/EvmcVm.java
    java/src/main/java/org/ethereum/evmc/Host.java
    java/src/main/java/org/ethereum/evmc/HostContext.java
    GENERATE_NATIVE_HEADERS evmc-java-native-headers)

add_library(evmc-java SHARED)
target_sources(evmc-java PRIVATE
    c/evmc-vm.c
    c/host.c
    c/host.h)
target_include_directories(evmc-java PRIVATE ${JNI_INCLUDE_DIRS})
target_link_libraries(evmc-java PRIVATE evmc-java-native-headers evmc::evmc evmc::loader ${JNI_LIBRARIES})

if(EVMC_INSTALL)
    install(TARGETS evmc-java EXPORT evmcTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install_jar(evmc-jar DESTINATION ${CMAKE_INSTALL_DATADIR}/java)
endif()