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

#ifndef REVERSINGSPACE_GAMEFILESYSTEM_FILE_HPP
#define REVERSINGSPACE_GAMEFILESYSTEM_FILE_HPP

#include <ReversingSpace/GameFileSystem/Core.hpp>
#include <ReversingSpace/Storage/Core.hpp>

#include <vector>

namespace reversingspace {
	namespace gfs {
		/**
		 * @brief File Interface (pure virtual)
		 *
		 * A File is an abstract (or pure virtual) base type designed to
		 * allow consistent access against multiple types.  Many of the
		 * file types defined will be abstract in nature, and thus will
		 * appear to be read-only, and may or may not allow seeking behaviour.
		 *
		 * This has been designed to allow a view to be mapped from Store,
		 * or to allow people to wrap an fstream (or similar) if they are so
		 * inclined.
		 */
		class File {
		public:
			/// Virtual destructor.
			virtual ~File() {}

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
			virtual storage::StorageSize seek(storage::StorageOffset offset,
				storage::Seek whence = storage::Seek::Set) = 0;

			/**
			 * @brief Gets the view size (in bytes).
			 * @return View size, in bytes.
			 *
			 * This is designed to be used with `get_data_pointer`.
			 */
			virtual storage::StorageSize get_size() const = 0;

			/**
			 * @brief Returns the current offset in the file.
			 */
			virtual storage::StorageOffset tell() const = 0;

			/**
			 * @brief Reads from the cursor position.
			 * @oaram[out] data Pointer to preallocated buffer (for storage).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 * 
			 * This method will lock the internal mutex for WRITE as it moves
			 * the cursor.
			 */
			virtual storage::StorageSize read(char* data,
				storage::StorageSize requested) = 0;

			/**
			 * @brief Reads data from a file into a vector.
			 * @oaram[out] data Vector for storing data.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 */
			virtual storage::StorageSize read(std::vector<std::uint8_t>& data,
				storage::StorageSize requested) = 0;

			/**
			 * @brief Reads from a specific offset in the view, storing it in the buffer.
			 * @param[in] offset  Offset (from the start of the view).
			 * @oaram[out] data Pointer to preallocated buffer (for storage).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 */
			virtual storage::StorageSize read_from(storage::StorageOffset offset,
				char* data, storage::StorageSize requested) = 0;

			/**
			 * @brief Reads data from a file into a vector.
			 * @param[in] offset  Offset (from the start of the view).
			 * @oaram[out] data Vector for storing data.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 */
			virtual storage::StorageSize read_from(storage::StorageOffset offset,
				std::vector<std::uint8_t>& data,
				storage::StorageSize requested) = 0;

			/**
			 * @brief Write a requested number of bytes into a file instance.
			 * This writes to the end of the data stream (using 'cursor').
			 * @param[in,out] data Pointer to data (buffer) to be written.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			virtual storage::StorageSize write(char* data,
				storage::StorageSize requested) = 0;

			/**
			 * @brief Reads data into a file from a vector.
			 * This writes to the end of the data stream (using 'cursor').
			 * @param[in,out] data Vector into which data is stored.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			virtual storage::StorageSize write(std::vector<std::uint8_t>& data,
				storage::StorageSize requested) = 0;

			/**
			 * @brief Write a requested number of bytes into a file instance.
			 * This writes to the given offset, only updating 'cursor' if it is exceeded.
			 * @param[in] offset  Offset (from the start of the view).
			 * @param[in,out] data Pointer to data (storage/buffer).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			virtual storage::StorageSize write_to(storage::StorageOffset offset,
				char* data, storage::StorageSize requested) = 0;

			/**
			 * @brief Reads data into a file from a vector.
			 * This writes to the given offset, only updating 'cursor' if it is exceeded.
			 * @param[in] offset  Offset (from the start of the view).
			 * @param[in,out] data Vector into which data is stored.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 */
			virtual storage::StorageSize write_to(storage::StorageOffset offset,
				std::vector<std::uint8_t>& data,
				storage::StorageSize requested) = 0;
		};
	}
}

#endif//REVERSINGSPACE_GAMEFILESYSTEM_FILE_HPP