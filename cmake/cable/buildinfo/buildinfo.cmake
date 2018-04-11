# Copyright 2018 Pawel Bylica.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

# Read the git commit hash from a file. The gitinfo is suppose to update the
# file only if the hash changes.
file(READ ${BINARY_DIR}/git_commit_hash.txt GIT_COMMIT_HASH)

string(TOLOWER ${SYSTEM_NAME} SYSTEM_NAME)
string(TOLOWER ${SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
string(TOLOWER ${COMPILER_ID} COMPILER_ID)
string(TOLOWER ${BUILD_TYPE} BUILD_TYPE)
string(TIMESTAMP TIMESTAMP)

message(
    "       Project Version:  ${PROJECT_VERSION}\n"
    "       System Name:      ${SYSTEM_NAME}\n"
    "       System Processor: ${SYSTEM_PROCESSOR}\n"
    "       Compiler ID:      ${COMPILER_ID}\n"
    "       Compiler Version: ${COMPILER_VERSION}\n"
    "       Build Type:       ${BUILD_TYPE}\n"
    "       Git Commit Hash:  ${GIT_COMMIT_HASH}\n"
    "       Timestamp:        ${TIMESTAMP}"
)

configure_file(${CMAKE_CURRENT_LIST_DIR}/buildinfo.c.in ${BINARY_DIR}/${NAME}.c)
