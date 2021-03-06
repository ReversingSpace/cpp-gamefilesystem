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

# Library name (as it appears as a binary)
set(LIBRARY_NAME "revspace-gfs")

# Suffix for static builds/work.
set(RS_GFX_STATIC_SUFFIX "-static")

# Shared library name
set(LIBRARY_SHARED_NAME "${LIBRARY_NAME}")

# Build name for the static copy of the library.
set(LIBRARY_STATIC_NAME "${LIBRARY_NAME}${RS_GFX_STATIC_SUFFIX}")

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

set(HEADERS_GAMEFILESYSTEM
    # Core include
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/Core.hpp"

    # API (common configuration/includes)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/API.hpp"

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
source_group("Header Files\\ReversingSpace\\GameFileSystem" FILES ${HEADERS_GAMEFILESYSTEM})

# Project root
set(HEADERS_STORAGE
    # Core (common configuration/includes)
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/Storage/Core.hpp"

    # File
    "${PROJECT_SOURCE_DIR}/include/ReversingSpace/Storage/File.hpp"
)
source_group("Header Files\\ReversingSpace\\Storage" FILES ${HEADERS_STORAGE})

set(SOURCES_STORAGE_COMMON
    # Common View code.
    "${PROJECT_SOURCE_DIR}/source/common/Storage/View.cpp"

    # Common File code.
    "${PROJECT_SOURCE_DIR}/source/common/Storage/File.cpp"

    # Platform File code.
    "${PROJECT_SOURCE_DIR}/source/common/GameFileSystem/PlatformFile.cpp"
)
source_group("Source Files\\Storage\\Common" FILES ${SOURCES_STORAGE_COMMON})

# Sources that are common to all platforms.
set(COMMON_SOURCES
    ${HEADERS_ROOT}
    ${HEADERS_STORAGE}
    ${HEADERS_GAMEFILESYSTEM}
    ${SOURCES_STORAGE_COMMON}
)

set(RS_GFS_PLATFORM_IS_POSIX 0)

# Windows
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(PLATFORM_SOURCES
        "${PROJECT_SOURCE_DIR}/source/Windows/Storage/File.cpp"
        "${PROJECT_SOURCE_DIR}/source/Windows/Storage/View.cpp"
    )
    source_group("Source Files\\Storage\\Windows" FILES ${PLATFORM_SOURCES})
endif()

# POSIX (common for a lot of platforms)
set(POSIX_SOURCES
    "${PROJECT_SOURCE_DIR}/source/POSIX/Storage/File.cpp"
    "${PROJECT_SOURCE_DIR}/source/POSIX/Storage/View.cpp"
)
source_group("Source Files\\Storage\\POSIX" FILES ${POSIX_SOURCES})

# macOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )
    set(RS_GFS_PLATFORM_IS_POSIX 1)
endif()

# Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )
    set(RS_GFS_PLATFORM_IS_POSIX 1)
endif()

# OpenBSD
if(${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
    set(PLATFORM_SOURCES
        ${POSIX_SOURCES}
    )
    set(RS_GFS_PLATFORM_IS_POSIX 1)
endif()

# ---------------------------------------------------------------------
# Library definition
# ---------------------------------------------------------------------

set(REVSPACE_GFS_SOURCES
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

if(${BUILD_STATIC_LIBS})
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
    if(${RS_GFS_PLATFORM_IS_POSIX})
        target_link_libraries(
            ${LIBRARY_STATIC_NAME}
            INTERFACE
            "stdc++fs"
            "pthread"
        )
    endif()
endif()

if(${BUILD_SHARED_LIBS})
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
    if(${RS_GFS_PLATFORM_IS_POSIX})
        target_link_libraries(
            ${LIBRARY_SHARED_NAME}
            PUBLIC
            "stdc++fs"
            "pthread"
        )
    endif()
endif()

# ---------------------------------------------------------------------
# Test Macro
# ---------------------------------------------------------------------

macro(rs_gfs_test conditional test_name test_include_dirs test_sources test_libs)
    if(${conditional})
        if(BUILD_STATIC_LIBS)
            set(TEST_NAME "${test_name}${RS_GFX_STATIC_SUFFIX}")
            message(STATUS "Building test: ${TEST_NAME}")
            add_executable(
                ${TEST_NAME}
                ${test_sources}
            )
            add_dependencies(
                ${TEST_NAME}
                ${LIBRARY_STATIC_NAME}
            )
            target_link_libraries(
                ${TEST_NAME}
                PRIVATE
                ${LIBRARY_STATIC_NAME}
                ${test_libs}
            )
            target_include_directories(
                ${TEST_NAME}
                PRIVATE
                ${test_include_dirs}
            )
            unset(TEST_NAME)
        endif()

        if(BUILD_SHARED_LIBS)
            set(TEST_NAME ${test_name})
            message(STATUS "Building test: ${TEST_NAME}")
            add_executable(
                ${TEST_NAME}
                ${test_sources}
            )
            add_dependencies(
                ${TEST_NAME}
                ${LIBRARY_SHARED_NAME}
            )
            target_link_libraries(
                ${TEST_NAME}
                PRIVATE
                ${LIBRARY_SHARED_NAME}
                ${test_libs}
            )
            target_include_directories(
                ${TEST_NAME}
                PRIVATE
                ${LIBRARY_SHARED_NAME}
            )
            unset(TEST_NAME)
        endif()
    endif()
endmacro()

# ---------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------

include("${PROJECT_SOURCE_DIR}/Tests.cmake")

# ---------------------------------------------------------------------
# Parent path detection.
# ---------------------------------------------------------------------

get_directory_property(IS_NOT_ROOT_DIRECTORY PARENT_DIRECTORY)
if(IS_NOT_ROOT_DIRECTORY)
    set(REVERSINGSPACE_GAMEFILESYSTEM_INCLUDE_DIR 
        "${PROJECT_SOURCE_DIR}/include" PARENT_SCOPE)
    set(REVERSINGSPACE_GAMEFILESYSTEM_STATIC_LIB 
        ${LIBRARY_STATIC_NAME} PARENT_SCOPE)
    if(BUILD_SHARED_LIBS)
        set(REVERSINGSPACE_GAMEFILESYSTEM_SHARED_LIB 
            ${LIBRARY_SHARED_NAME} PARENT_SCOPE)
    endif()
endif()