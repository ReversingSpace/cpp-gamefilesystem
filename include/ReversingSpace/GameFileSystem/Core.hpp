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

#ifndef REVERSINGSPACE_GAMEFILESYSTEM_CORE_HPP
#define REVERSINGSPACE_GAMEFILESYSTEM_CORE_HPP

#include <ReversingSpace/GameFileSystem/API.hpp>

// std::uint64_t
#include <cstdint>

// std::filesystem
#include <filesystem>

// std::function
#include <functional>

// std::shared_ptr
#include <memory>

// std::string
#include <string>

namespace reversingspace {
	namespace gfs {

		/// If the file is below this size it will be completely mapped
		/// as a view (256 * KB * B -> 256MB).
		const std::uint64_t AUTO_FULL_MAP_SIZE = 256 * 1024 * 1024;

		/// Hashed Identity type.
		using HashedIdentity = std::uint64_t;

		/// String identity.
		using StringIdentity = const std::string&;

		/**
		 * @brief Function used for hashing.
		 */
		using HashFunction = std::function<HashedIdentity(StringIdentity name)>;

		// Forward for `File`.
		class REVSPACE_GAMEFILESYSTEM_API File;

		/// Shared pointer type for `File`.
		using FilePointer = std::shared_ptr<File>;

		// Forward for `Directory`.
		template<class UserlandFileType>
		class Directory;

		/// Shared pointer type for `Object`.
		template<class UserlandFileType>
		using DirectoryPointer = std::shared_ptr<Directory<UserlandFileType>>;

		// Forward for `FileSystem`.
		class REVSPACE_GAMEFILESYSTEM_API FileSystem;

		/// Shared pointer type for `FileSystem`.
		using FileSystemPointer = std::shared_ptr<FileSystem>;

		// Forward for `Archive`.
		class REVSPACE_GAMEFILESYSTEM_API Archive;

		/// Shared pointer type for `File`.
		using ArchivePointer = std::shared_ptr<Archive>;

		// Forward for `StorageServer`.
		template<class UserlandFileType>
		class REVSPACE_GAMEFILESYSTEM_API StorageServer;

		/// Shared pointer type for `StorageServer`.
		// This should almost never be used, but it's here for completeness
		// and to support a Rust-style constructor (`StorageServer::create`).
		template<class UserlandFileType>
		using StorageServerPointer = std::shared_ptr<StorageServer<UserlandFileType>>;
	}
}

#endif//REVERSINGSPACE_STORAGE_CORE_HPP