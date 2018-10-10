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

#ifndef REVERSINGSPACE_GAMEFILESYSTEM_FILESYSTEM_HPP
#define REVERSINGSPACE_GAMEFILESYSTEM_FILESYSTEM_HPP

#include <ReversingSpace/GameFileSystem/Core.hpp>
#include <ReversingSpace/Storage/Core.hpp>

namespace reversingspace {
	namespace gfs {
		/**
		 * @brief FileSystem Interface (pure virtual)
		 *
		 * A FileSystem, or virtual file system, represents a system by
		 * which files are sorted, stored, and made accessible.
		 *
		 * The more traditional systems are loose files on an underlying
		 * (platform driven) filesystem, such as FAT, NTFS, ext2/3/4, or HFS.
		 * This definition allows for that kind of access (if desired), but
		 * also allows for more complex (pre-hashed) file references.
		 */
		class FileSystem {
		public:
			/// Virtual destructor.
			virtual ~FileSystem() {}

		public: // Core requirements.

			/**
			 * @brief Gets the filesystem path of the filesystem.
			 */
			virtual std::filesystem::path get_path() const = 0;
			 
			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 */
			virtual FilePointer get_file(HashedIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) = 0;

			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 */
			virtual FilePointer get_file(StringIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) = 0;
		};
	}
}

#endif//REVERSINGSPACE_GAMEFILESYSTEM_FILESYSTEM_HPP