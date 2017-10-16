/*
 * Copyright 2017 ReversingSpace. See the COPYRIGHT file at the
 * top-level directory of this distribution and in the repository:
 * https://github.com/ReversingSpace/cpp-gamefilesystem
 *
 * Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
 * http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
 * <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
 * option. This file may not be copied, modified, or distributed
 * except according to those terms.
**/

#ifndef __REVERSINGSPACE__GAMEFILESYSTEM__API_HPP__
#define __REVERSINGSPACE__GAMEFILESYSTEM__API_HPP__

/**
 * Regarding Windows:
 * GAMEFILESYSTEM_SHARED is only defined when building the shared library.
 * gcc and clang both support __declspec, so we'll just use that.
**/
#if defined(_WIN32) || defined(_WIN64)
#	if defined(GAMEFILESYSTEM_SHARED) // DLL export
#		define GAMEFILESYSTEM_API __declspec(dllexport)
#	elif defined(GAMEFILESYSTEM_STATIC) // Static
#		define GAMEFILESYSTEM_API // Fallback to nothing is fine.
#	else // DLL import
#		define GAMEFILESYSTEM_API __declspec(dllimport)
#	endif
#	if defined(_MSC_VER) // This is the only point where it varies on Windows.
#		define GAMEFILESYSTEM_API_LOCAL
#	else
#		define	GAMEFILESYSTEM_API_LOCAL __attribute__((visibility("hidden")))
#	endif
#else
#	define GAMEFILESYSTEM_API __attribute__((visibility("default")))
#	define GAMEFILESYSTEM_API_LOCAL __attribute__((visibility("hidden")))
#endif

#endif//__REVERSINGSPACE__GAMEFILESYSTEM__API_HPP__