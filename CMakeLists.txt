# ---------------------------------------------------------------------
# Copyright (c) 2017 ReversingSpace.
# This file is released into the public domain, the MIT Licence, or
# the Apache Licence 2.0 (at your choice).
# ---------------------------------------------------------------------

# Travis CI is being used as the low bar for compiler support on macOS
# and Linux.  Windows 10 is now the lowest supported version of Windows.
#
# Some versions for minimum support:
# - Visual C++ 2017 runtime;
# - gcc 4.8.4;
# - Clang 3.9.
#
# Anything earlier can be handled with preprocessors.

# Minimum CMake version must be defined early.
cmake_minimum_required(VERSION 3.0)

# Project identification must happen early too.
project(cpp_game_filesystem)

# Adding C++20 support when it's available.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------
# Project definitions
# - Includes CMake configuration related code.
# ---------------------------------------------------------------------

option(
    REVERSINGSPACE_STORAGE_TEST_INTERNALS
    "Simple test for storage system (internals)"
    OFF
)

option(
    REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW
    "Simple r/w test for storage system"
    OFF
)

# Build name for the static copy of the library.
set(LIBRARY_STATIC_NAME "game-filesystem-static")

# ---------------------------------------------------------------------
# Soure code listings
#
# It is possible to collapse this into if/else groupings that are
# slightly faster.  This is easier to maintain.
# ---------------------------------------------------------------------

# Header files are split out a lot more, making it far neater in IDEs
# that CMake supports doing layout/source group work in.  This is
# really just for effect.
#  'Header Files\\'
set(HEADERS_ROOT
    # Root level header.  It's the "include me if you're lazy" file.
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem.hpp"
    
)
source_group("Header Files\\ReversingSpace" FILES ${HEADERS_ROOT})

# Project root
set(HEADERS_PROJECT
    # No build config here (todo?)
    # Config file would be a CMake build.

    # Core (common configuration/includes)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/Storage/Core.hpp"

    # Directory
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/Storage/Directory.hpp"

    # File
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/Storage/File.hpp"
)
source_group("Header Files\\reversingspace\\storage" FILES ${HEADERS_PROJECT})

# Sources that are common to all platforms.
set(COMMON_SOURCES
    ${HEADERS_ROOT}
    ${HEADERS_PROJECT}

    # Common View code.
    "${PROJECT_SOURCE_DIR}/source/common/View.cpp"

    # Common File code.
    "${PROJECT_SOURCE_DIR}/source/common/File.cpp"

    # Common Directory code.
    "${PROJECT_SOURCE_DIR}/source/common/Directory.cpp"
)

# Shared sources that are common to all platforms.
set(SHARED_COMMON_SOURCES

)

# Static sources that are common to all platforms.
set(STATIC_COMMON_SOURCES

)

# Windows
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(PLATFORM_SOURCES
        "${PROJECT_SOURCE_DIR}/source/Windows/File.cpp"
        "${PROJECT_SOURCE_DIR}/source/Windows/View.cpp"
    )

    set(SHARED_PLATFORM_SOURCES
        
    )

    set(STATIC_PLATFORM_SOURCES
    
    )
endif()

set(POSIX_SOURCES
    "${PROJECT_SOURCE_DIR}/source/POSIX/File.cpp"
    "${PROJECT_SOURCE_DIR}/source/POSIX/View.cpp"
)

# macOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )

    set(SHARED_PLATFORM_SOURCES
    
    )

    set(STATIC_PLATFORM_SOURCES
    
    )
endif()

# Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )

    set(SHARED_PLATFORM_SOURCES
    
    )

    set(STATIC_PLATFORM_SOURCES
    
    )
endif()

# OpenBSD
if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )

    set(SHARED_PLATFORM_SOURCES

    )

    set(STATIC_PLATFORM_SOURCES

    )
endif()

# ---------------------------------------------------------------------
# Library definition
# ---------------------------------------------------------------------

add_library(
    ${LIBRARY_STATIC_NAME}
    STATIC
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
    ${STATIC_PLATFORM_SOURCES}
    ${STATIC_COMMON_SOURCES}
)
target_include_directories(
    ${LIBRARY_STATIC_NAME}
    PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

# ---------------------------------------------------------------------
# Internals test
# ---------------------------------------------------------------------

if(REVERSINGSPACE_STORAGE_TEST_INTERNALS)
    set(REVERSINGSPACE_STORAGE_TEST_INTERNALS_BINARY_NAME
        "revspace-storage-test-internals"
    )
    add_executable(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_BINARY_NAME}
        "${PROJECT_SOURCE_DIR}/tests/internals/main.cpp"
    )
    add_dependencies(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_BINARY_NAME}
        ${LIBRARY_STATIC_NAME}
    )
    target_include_directories(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_BINARY_NAME}
        PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/tests/internals/"
    )
    target_link_libraries(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_BINARY_NAME}
        PRIVATE
        ${LIBRARY_STATIC_NAME}
    )

    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(
            ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_BINARY_NAME}
            PRIVATE
            "stdc++fs"
            "pthread"
        )
    endif()

    if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        target_link_libraries(
            ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_BINARY_NAME}
            PRIVATE
            "stdc++fs"
            "pthread"
        )
    endif()
endif()

if(REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW)
    set(REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW_BINARY_NAME
        "revspace-storage-test-internals-rw"
    )
    add_executable(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW_BINARY_NAME}
        "${PROJECT_SOURCE_DIR}/tests/internals-rw/main.cpp"
    )
    add_dependencies(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW_BINARY_NAME}
        ${LIBRARY_STATIC_NAME}
    )
    target_include_directories(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW_BINARY_NAME}
        PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/tests/internals-rw/"
    )
    target_link_libraries(
        ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW_BINARY_NAME}
        PRIVATE
        ${LIBRARY_STATIC_NAME}
    )

    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(
            ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW_BINARY_NAME}
            PRIVATE
            "stdc++fs"
            "pthread"
        )
    endif()

    if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        target_link_libraries(
            ${REVERSINGSPACE_STORAGE_TEST_INTERNALS_RW_BINARY_NAME}
            PRIVATE
            "stdc++fs"
            "pthread"
        )
    endif()
endif()
