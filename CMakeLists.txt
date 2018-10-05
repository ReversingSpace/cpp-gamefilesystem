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
# - gcc 8.0;
# - Clang (unknown).
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
# Project definitions and options.
# - Includes CMake configuration related code.
# ---------------------------------------------------------------------

option(BUILD_SHARED_LIBS "Build the shared library (dll, so, dylib)" ON)
option(BUILD_STATIC_LIBS "Build the static library" ON)

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

# Shared library name
set(LIBRARY_SHARED_NAME "revspace-gfs")

# Build name for the static copy of the library.
set(LIBRARY_STATIC_NAME "revspace-gfs-static")

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

    # Root include
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem.hpp"

    # API (common configuration/includes)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/API.hpp"

    # Core (common configuration/includes)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/Storage/Core.hpp"

    # File
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/Storage/File.hpp"

    # Storage Server
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/StorageServer.hpp"

    # Archive (Interface)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/Archive.hpp"

    # FileSystem (Interface)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/FileSystem.hpp"

    # FileSystem File (interface)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/File.hpp"

    # FileSystem File (implementation)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/PlatformFile.hpp"

    # Directory
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/Directory.hpp"
)
source_group("Header Files\\reversingspace\\storage" FILES ${HEADERS_PROJECT})

# Sources that are common to all platforms.
set(COMMON_SOURCES
    ${HEADERS_ROOT}
    ${HEADERS_PROJECT}

    # Common View code.
    "${PROJECT_SOURCE_DIR}/source/common/Storage/View.cpp"

    # Common File code.
    "${PROJECT_SOURCE_DIR}/source/common/Storage/File.cpp"

    # Platform File code.
    "${PROJECT_SOURCE_DIR}/source/common/GameFileSystem/PlatformFile.cpp"
)

# Windows
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(PLATFORM_SOURCES
        "${PROJECT_SOURCE_DIR}/source/Windows/Storage/File.cpp"
        "${PROJECT_SOURCE_DIR}/source/Windows/Storage/View.cpp"
    )
endif()

# POSIX (common for a lot of platforms)
set(POSIX_SOURCES
    "${PROJECT_SOURCE_DIR}/source/POSIX/Storage/File.cpp"
    "${PROJECT_SOURCE_DIR}/source/POSIX/Storage/View.cpp"
)

# macOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )
endif()

# Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )
endif()

# OpenBSD
if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )
endif()

# ---------------------------------------------------------------------
# Library definition
# ---------------------------------------------------------------------

set(REVSPACE_GFS_SOURCES
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

if(BUILD_STATIC_LIBS)
    add_library(
        ${LIBRARY_STATIC_NAME}
        STATIC
        ${REVSPACE_GFS_SOURCES}
    )
    target_include_directories(
        ${LIBRARY_STATIC_NAME}
        PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/source"
        INTERFACE
        "${PROJECT_SOURCE_DIR}/include"
    )
    target_compile_definitions(
        ${LIBRARY_STATIC_NAME}
        PRIVATE
        "REVSPACE_GAMEFILESYSTEM_STATIC=1"
        INTERFACE
        "REVSPACE_GAMEFILESYSTEM_STATIC=1"
    )
endif()

if(BUILD_SHARED_LIBS)
    add_library(
        ${LIBRARY_SHARED_NAME}
        SHARED
        ${REVSPACE_GFS_SOURCES}
    )
    target_include_directories(
        ${LIBRARY_SHARED_NAME}
        PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/source"
        INTERFACE
        "${PROJECT_SOURCE_DIR}/include"
    )
    target_compile_definitions(
        ${LIBRARY_SHARED_NAME}
        PRIVATE
        "REVSPACE_GAMEFILESYSTEM_SHARED=1"
    )
endif()

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
