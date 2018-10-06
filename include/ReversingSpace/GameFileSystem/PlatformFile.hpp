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

#ifndef REVERSINGSPACE_GAMEFILESYSTEM_PLATFORMFILE_HPP
#define REVERSINGSPACE_GAMEFILESYSTEM_PLATFORMFILE_HPP

#include <ReversingSpace/GameFileSystem/Core.hpp>
#include <ReversingSpace/GameFileSystem/File.hpp>
#include <ReversingSpace/Storage/File.hpp>

#include <shared_mutex>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4251)
#endif//defined(_MSC_VER)

namespace reversingspace {
	namespace gfs {
		/**
		 * @brief Storage backed Platform File.
		 *
		 * This pulls views on the fly to perform its operations, so it
		 * functions in a non-insert mode.  This is kind of less than ideal
		 * and you should create an archive type to handle this better
		 * (where possible).
		 */
		class REVSPACE_GAMEFILESYSTEM_API PlatformFile : public File {
		private:
			/// Memory-mapped file.
			storage::FilePointer stored_file;

			/// Cursor
			storage::StorageSize cursor;

			/// Read/write mutex.
			std::shared_mutex rw_mutex;

		public: // 'create' to allow this to be used by StorageServer

			/**
			 * @brief Gets the underlying stored file.
			 * @return Shared pointer to a `storage::FilePointer`.
			 *
			 * This should be used with great care, mostly because it will
			 * require a cast (and dynamic_casting isn't free!).
			 */
			inline storage::FilePointer get_stored_file() {
				return stored_file;
			}

			/**
			 * @brief Inline static helper to construct a PlatformFile.
			 * @param[in] path    Path to file.
			 * @param[in] access  Requested access to file.
			 * @return shared_ptr to a PlatformFile, or nullptr on failure.
			 */
			inline static PlatformFilePointer create(const std::filesystem::path& path,
				storage::FileAccess access = storage::FileAccess::Read) {
				
				// Get a stored file.
				// This leaves issue #2 (symlinking) an upstream issue
				// and not something that needs re-addressing here!
				auto stored = storage::File::create(path, access);
				if (stored == nullptr) {
					return nullptr;
				}

				// Create and return.
				auto file = std::make_shared<PlatformFile>();
				file->stored_file = stored;
				file->cursor = 0;

				return file;
			}

		public: // File

			/**
			 * Sets the cursor position.
			 *
			 * This method will lock the internal mutex for WRITE as it modifies
			 * the internal cursor.
			 *
			 * @param[in] offset Offset from the point of origin.
			 * @param[in] whence Point of origin ('whence') for the seek.
			 * @return Current offset (unsigned as it canno be negative).
			 */
			storage::StorageSize seek(storage::StorageOffset offset,
				storage::Seek whence = storage::Seek::Set);

			/**
			 * @brief Gets the view size (in bytes).
			 * @return View size, in bytes.
			 *
			 * This is designed to be used with `get_data_pointer`.
			 */
			storage::StorageSize get_size() const {
				return stored_file->get_size();
			}

			/**
			 * @brief Returns the current offset in the file.
			 */
			inline storage::StorageOffset tell() const {
				return cursor;
			}

			/**
			 * @brief Reads from the cursor position.
			 * @oaram[out] data Pointer to preallocated buffer (for storage).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 *
			 * This method will lock the internal mutex for WRITE as it moves
			 * the cursor.
			 */
			storage::StorageSize read(char* data,
				storage::StorageSize requested);

			/**
			 * @brief Reads data from a file into a vector.
			 * @oaram[out] data Vector for storing data.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 */
			storage::StorageSize read(std::vector<std::uint8_t>& data,
				storage::StorageSize requested);

			/**
			 * @brief Reads from a specific offset in the view, storing it in the buffer.
			 * @param[in] offset  Offset (from the start of the view).
			 * @oaram[out] data Pointer to preallocated buffer (for storage).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 */
			storage::StorageSize read_from(storage::StorageOffset offset,
				char* data, storage::StorageSize requested);

			/**
			 * @brief Reads data from a file into a vector.
			 * @param[in] offset  Offset (from the start of the view).
			 * @oaram[out] data Vector for storing data.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 */
			storage::StorageSize read_from(storage::StorageOffset offset,
				std::vector<std::uint8_t>& data,
				storage::StorageSize requested);

			/**
			 * @brief Write a requested number of bytes into a file instance.
			 * This writes to the end of the data stream (using 'cursor').
			 * @param[in,out] data Pointer to data (buffer) to be written.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			storage::StorageSize write(char* data,
				storage::StorageSize requested);

			/**
			 * @brief Reads data into a file from a vector.
			 * This writes to the end of the data stream (using 'cursor').
			 * @param[in,out] data Vector into which data is stored.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			storage::StorageSize write(std::vector<std::uint8_t>& data,
				storage::StorageSize requested);

			/**
			 * @brief Write a requested number of bytes into a file instance.
			 * This writes to the given offset, only updating 'cursor' if it is exceeded.
			 * @param[in] offset  Offset (from the start of the view).
			 * @param[in,out] data Pointer to data (storage/buffer).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			storage::StorageSize write_to(storage::StorageOffset offset,
				char* data, storage::StorageSize requested);

			/**
			 * @brief Reads data into a file from a vector.
			 * This writes to the given offset, only updating 'cursor' if it is exceeded.
			 * @param[in] offset  Offset (from the start of the view).
			 * @param[in,out] data Vector into which data is stored.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			storage::StorageSize write_to(storage::StorageOffset offset,
				std::vector<std::uint8_t>& data,
				storage::StorageSize requested);
		};
	}
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif//defined(_MSC_VER)

#endif//REVERSINGSPACE_GAMEFILESYSTEM_PLATFORMFILE_HPP