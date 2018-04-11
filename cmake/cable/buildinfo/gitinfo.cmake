# Copyright 2018 Pawel Bylica.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

# Execute git only if the tool is available.
if(GIT)
    execute_process(
        COMMAND ${GIT} rev-parse HEAD
        WORKING_DIRECTORY ${SOURCE_DIR}
        OUTPUT_VARIABLE commit_hash
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

set(commit_hash_file ${BINARY_DIR}/git_commit_hash.txt)

if(EXISTS ${commit_hash_file})
    file(READ ${commit_hash_file} prev_commit_hash)
else()
    # Create empty file, because other targets expect it to exist.
    file(WRITE ${commit_hash_file} "")
endif()

if(NOT "${commit_hash}" STREQUAL "${prev_commit_hash}")
    file(WRITE ${commit_hash_file} ${commit_hash})
endif()
