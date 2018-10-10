/*
 * Copyright 2017-2018 ReversingSpace. See the COPYRIGHT file at the
 * top-level directory of this distribution and in the repository:
 * https://github.com/ReversingSpace/cpp-gamefilesystem
 *
 * Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
 * http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
 * <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
 * option. This file may not be copied, modified, or distributed
 * except according to those terms.
**/

/**
 * @file Core.hpp
 * @brief Common configuration and types.
**/

#ifndef REVERSINGSPACE_STORAGE_CORE_HPP
#define REVERSINGSPACE_STORAGE_CORE_HPP

#include <ReversingSpace/GameFileSystem/API.hpp>

// std::uint8_t
#include <cstdint>

// std::filesystem
#include <filesystem>

// std::shared_ptr
#include <memory>

namespace reversingspace {
#if !defined(GET_PLATFORM_GRANULARITY)
	namespace platform {
		std::uint64_t get_granularity();
	}
#endif
	namespace storage {

		// Forward for `File`.
		class REVSPACE_GAMEFILESYSTEM_API File;

		/// Shared pointer type for `File`.
		using FilePointer = std::shared_ptr<File>;

		// Forward for `Object`.
		class REVSPACE_GAMEFILESYSTEM_API View;

		/// Shared pointer type for `Object`.
		using ViewPointer = std::shared_ptr<View>;

		/// Storage format (to keep it consistent) for offset(s).
		using StorageOffset = std::int64_t;

		/// Storage format (to keep it consistent) for size.
		using StorageSize = std::uint64_t;

#if defined(_WIN32) || defined(_WIN64)
		/// Windows Handle alias (HANDLE).
		using PlatformFileHandle = void*;

		/// Platform Invalid File Handle (indicating the file is invalid).
		const PlatformFileHandle PLATFORM_INVALID_FILE_HANDLE = (void*)-1;
#else
		/// Non-Windows Handle alias (file descriptor).
		using PlatformFileHandle = int;

		/// Platform Invalid File Handle (indicating the file is invalid).
		const PlatformFileHandle PLATFORM_INVALID_FILE_HANDLE = -1;
#endif

		/**
		 * @brief Access mode for a file.
		 *
		 * This is relatively limited (for now at least) as execution is
		 * not something that should be happening (at least in the imagined
		 * definitions of the mapping system).
		 */
		enum class FileAccess : std::uint8_t {
			/**
			 * @brief Read access.
			 *
			 * This should be enabled if there is any chance the file needs to
			 * to read or have access to the data.
			 */
			Read = 1 << 0,

			/**
			 * @brief Write access.
			 *
			 * If the data needs to be modified, this must be set.
			 */
			Write = 1 << 1,
			
			/**
			 * @brief Execution access.
			 *
			 * Not yet enabled/used.  This is here as a reservation.
			 **/
			Execute = 1 << 2,

			/**
			 * @brief Read and Write (alias to overcome enum class errors)
			 *
			 * Read and write merged access.  See descriptors for
			 * `Read` and `Write`.
			 **/
			ReadWrite = Read | Write,

			/**
			 * @brief Read and Execute access.
			 */
			ReadExecute = Read | Execute,

			/**
			 * @brief Alias for `Read`, `Write`, and `Execute`.
			 */
			 ReadWriteExecute = Read | Write | Execute,
		};

		/**
		 * @brief Seek points of origin.
		 *
		 * Offset for seeking in files regardless of underlying system.
		 * This is mostly a relic of the old system.
		 */
		enum class Seek : std::uint8_t {
			/// Beginning of File/Stream.
			Set = 0,
			
			/// Current position.
			Current,

			/// Final position
			End,

			/// Alias for 'Set'.
			Start = Set,

			/// Alias for 'Set'.
			Beginning = Set,
		};
	}
}

#if !defined(GET_PLATFORM_GRANULARITY)
#define GET_PLATFORM_GRANULARITY() reversingspace::platform::get_granularity()
#define REVERSINGSPACE_STORAGE_GRANULARITY 1
#endif

#endif//REVERSINGSPACE_STORAGE_CORE_HPP