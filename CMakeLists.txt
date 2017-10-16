# ---------------------------------------------------------------------
# Copyright (c) 2017 ReversingSpace.
# This file is released into the public domain, the MIT Licence, or
# the Apache Licence 2.0 (at your choice).
# ---------------------------------------------------------------------

# Travis CI is being used as the low bar for compiler support on macOS
# and Linux.  Windows 10 is the lowest supported version of Windows,
# as 8.1 hits end of mainstream in January 2018.
#
# Some versions for minimum support:
# - Visual C++ 2017 runtime;
# - gcc 4.8.4;
# - Clang 3.9.
#
# Anything earlier can be handled with preprocessors.

# Minimum CMake version must be defined early.
cmake_minimum_required(VERSION 3.2)

# Project identification must happen early too.
project(cpp_game_filesystem)

# ---------------------------------------------------------------------
# Project options (what users need to mess with)
# ---------------------------------------------------------------------

# Build the static version of the library?
option(BUILD_STATIC_LIBS "Build the static library" ON)

# Build the shared version of the library?
option(BUILD_SHARED_LIBS "Build the shared library" ON)

# ---------------------------------------------------------------------
# Project definitions
# - Includes CMake configuration related code.
# ---------------------------------------------------------------------

# Build name for the shared copy of the library.
set(library_shared_name "game-filesystem")

# Build name for the static copy of the library.
set(library_static_name "game-filesystem-static")

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
    "${CMAKE_SOURCE_DIR}/include/ReversingSpace/GameFileSystem.hpp"
    
)
source_group("Header Files\\ReversingSpace" FILES ${HEADERS_ROOT})

# Project root
set(HEADERS_PROJECT
    # API file (used quite a bit)
    "${CMAKE_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/API.hpp"

    # No build config here (todo?)
    # Config file would be a CMake build.

    # Directory
    "${CMAKE_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/Directory.hpp"

    # File
    "${CMAKE_SOURCE_DIR}/include/ReversingSpace/GameFileSystem/File.hpp"
)
source_group("Header Files\\ReversingSpace\\GameFileSystem" FILES ${HEADERS_PROJECT})

# Sources that are common to all platforms.
set(COMMON_SOURCES
    ${HEADERS_ROOT}
    ${HEADERS_PROJECT}
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
        "${CMAKE_SOURCE_DIR}/source/Windows/File.cpp"
    )

    set(SHARED_PLATFORM_SOURCES
        
    )

    set(STATIC_PLATFORM_SOURCES
    
    )
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# macOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PLATFORM_SOURCES
        "${CMAKE_SOURCE_DIR}/source/Darwin/File.cpp"
    )

    set(SHARED_PLATFORM_SOURCES
    
    )

    set(STATIC_PLATFORM_SOURCES
    
    )
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_SOURCES
        "${CMAKE_SOURCE_DIR}/source/Linux/File.cpp"
    )

    set(SHARED_PLATFORM_SOURCES
    
    )

    set(STATIC_PLATFORM_SOURCES
    
    )
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# OpenBSD (not yet supported, but listed to remind me/us).
#IF({CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
#ENDIF({CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")

# ---------------------------------------------------------------------
# Library definitions
# ---------------------------------------------------------------------

if(${BUILD_SHARED_LIBS})
    add_library(
        ${library_shared_name}
        SHARED
        ${COMMON_SOURCES}
        ${PLATFORM_SOURCES}
        ${SHARED_PLATFORM_SOURCES}
        ${SHARED_COMMON_SOURCES}
    )
    target_include_directories(
        ${library_shared_name}
        PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
    )
    target_compile_definitions(
        ${library_shared_name}
        PUBLIC
        -DGAMEFILESYSTEM_SHARED=1
    )
endif(${BUILD_SHARED_LIBS})

if(${BUILD_STATIC_LIBS})
    add_library(
        ${library_static_name}
        STATIC
        ${COMMON_SOURCES}
        ${PLATFORM_SOURCES}
        ${STATIC_PLATFORM_SOURCES}
        ${STATIC_COMMON_SOURCES}
    )
    target_include_directories(
        ${library_static_name}
        PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
    )
endif(${BUILD_STATIC_LIBS})