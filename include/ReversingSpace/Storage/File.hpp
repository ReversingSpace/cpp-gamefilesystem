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
 * @file File.hpp
 * @brief Virtual Object definition, and File (filesystem).
**/

#ifndef REVERSINGSPACE_STORAGE_FILE_HPP
#define REVERSINGSPACE_STORAGE_FILE_HPP

// API
#include <ReversingSpace/Storage/Core.hpp>

// std::shared_mutex
#include <shared_mutex>

// A vector is used for I/O from the View structure.
#include <vector>

namespace reversingspace {
    namespace storage {
		/**
		 * @brief Virtual FileSystem object/entry/entity.
		 *
		 * This is the 'actual' object type.  In other code this may be read
		 * as a 'map' or a 'view'.  It is called `View` here as it is a view
		 * into a given map.
		 */
		class REVSPACE_GAMEFILESYSTEM_API View {
		private:
			/**
			 * @brief Allow `File` to create without exposing.
			 */
			friend class File;

#if defined(_WIN32) || defined(_WIN64)
			/**
			 * @brief Handle to the file mapping.
			 *
			 * This is required on a per-map/view/object basis because of how
			 * the internal system here works.  Otherwise changes to the file
			 * size could (would/should?) result in unstable code.
			 */
			PlatformFileHandle file_map_handle;
#endif

			/**
			 * @brief Read/Write Mutex.
			 */
			std::shared_mutex rw_mutex;

			/**
			 * @brief File from which this view is created.
			 */
			FilePointer file;

			/**
			 * @brief Pointer to the mapped data.
			 *
			 * Any/all manipulation must happen here.
			 **/
			void* view_pointer;

			/**
			 * @brief Offset within the file.
			 */
			size_t file_offset;

			/**
			 * @brief Length of the mapped data (in bytes).
			 *
			 * This is the "mapped size".
			 **/
			size_t view_length;

			// Note: File size can be fetched from the file.

			/**
			 * @brief Offset (cursor) for read/write actions.
			 *
			 * Following the C standard, this is a shared cursor
			 * for both read and write operations.
			 */ 
			StorageOffset cursor;

			/** 
			 * @brief Internal function to open a map.
			 *
			 * This is platform-specific code, used within `File`.
			 */
			bool open_mapping();

		public: // Preliminary API
			/**
			 * @brief Flushes the mapped region to disk (if possible).
			 *
			 * This can only happen if the underlying file is open
			 * with write access.
			 */
			bool flush();

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
			StorageSize seek(StorageOffset offset, Seek whence = Seek::Set);

			/**
			 * @brief Deconstructor.
			 *
			 * Designed to enable shared_ptr usage (or reckless abandon).
			 */
			~View();

		public: // Read/Write + Helpers for I/O.

			/**
			 * @brief Gets raw access to the data pointer.
			 * @return `view_pointer` (the raw data pointer).
			 *
			 * This should typically not be required, and is provided only
			 * to allow for more advanced usage than is typically required.
			 */
			inline void* get_data_pointer() {
				return view_pointer;
			}

			/**
			 * @brief Gets the view size (in bytes).
			 * @return View size, in bytes.
			 *
			 * This is designed to be used with `get_data_pointer`.
			 */
			inline StorageSize get_size() const {
				return view_length;
			}

			/**
			 * @brief Gets the absolute offset in the file.
			 * @return Sum of cursor and file offset (absolute offset).
			 */
			inline StorageSize get_absolute_offset() const {
				return cursor + file_offset;
			}

			/**
			 * @brief Gets the file offset (how far into the file the view is).
			 * @return File offset
			 */
			inline StorageSize get_file_offset() const {
				return file_offset;
			}

			/**
			 * @brief Gets offset (within view/cursor).
			 * @return The internal view offset (or cursor).
			 */
			inline StorageSize get_offset() const {
				return cursor;
			}

			/**
			 * @brief Calculates size allowances.
			 * @param[in] offset      Offset from which to calculat.
			 * @param[in] requested   Number of bytes requested.
			 * @return Number of bytes which can be written or read.
			 *
			 * This method calculates the maximum number of bytes which can be
			 * written to, or read from, based on a given offset and requested
			 * size.  This uses the length of the mapped view internally and
			 * is designed to reduce issues with over-writing.
			 *
			 * This is made public to enable testing prior to writing (though
			 * the system will test internally again, so it's best to just write
			 * or read and then re-issue the command).
			 */
			StorageSize calculate_allowance(StorageSize offset, StorageSize requested);

			/**
			 * @brief Reads from the cursor position.
			 * @oaram[out] data Pointer to preallocated buffer (for storage).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 * 
			 * This method will lock the internal mutex for WRITE as it moves
			 * the cursor.
			 */
			StorageSize read(char* data, StorageSize requested);

			/**
			 * @brief Reads data from a file into a vector.
			 * @oaram[out] data Vector for storing data.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 * 
			 * This method will lock the internal mutex for WRITE as it moves
			 * the cursor.
			 */
			StorageSize read(std::vector<std::uint8_t>& data, StorageSize requested);

			/**
			 * @brief Reads from a specific offset in the view, storing it in the buffer.
			 * @param[in] offset  Offset (from the start of the view).
			 * @oaram[out] data Pointer to preallocated buffer (for storage).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 * 
			 * This method will NOT lock.
			 */
			StorageSize read_from(StorageOffset offset, char* data, StorageSize requested);

			/**
			 * @brief Reads data from a file into a vector.
			 * @param[in] offset  Offset (from the start of the view).
			 * @oaram[out] data Vector for storing data.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes read.
			 * 
			 * This method will NOT lock.
			 */
			StorageSize read_from(StorageOffset offset, std::vector<std::uint8_t>& data, StorageSize requested);

			/**
			 * @brief Write a requested number of bytes into a file instance.
			 * This writes to the end of the data stream (using 'cursor').
			 * @param[in,out] data Pointer to data (buffer) to be written.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 * 
			 * This method will lock the internal mutex for WRITE.
			 */
			StorageSize write(char* data, StorageSize requested);

			/**
			 * @brief Reads data into a file from a vector.
			 * This writes to the end of the data stream (using 'cursor').
			 * @param[in,out] data Vector into which data is stored.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 * 
			 * This method will lock the internal mutex for WRITE.
			 */
			StorageSize write(std::vector<std::uint8_t>& data, StorageSize requested);

			/**
			 * @brief Write a requested number of bytes into a file instance.
			 * This writes to the given offset, only updating 'cursor' if it is exceeded.
			 * @param[in] offset  Offset (from the start of the view).
			 * @param[in,out] data Pointer to data (storage/buffer).
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 * 
			 * This method will NOT lock.
			 */
			StorageSize write_to(StorageOffset offset, char* data, StorageSize requested);

			/**
			 * @brief Reads data into a file from a vector.
			 * This writes to the given offset, only updating 'cursor' if it is exceeded.
			 * @param[in] offset  Offset (from the start of the view).
			 * @param[in,out] data Vector into which data is stored.
			 * @param[in] requested Number of bytes requested.
			 * @return number of bytes written.
			 * 
			 * This method will NOT lock.
			 */
			StorageSize write_to(StorageOffset offset, std::vector<std::uint8_t>& data, StorageSize requested);
		};

        /**
         * @brief Object on a filesystem.
         * 
         * A File object represents what the standard calls a 'regular_file'.
		 *
		 * The constructor is not hidden, but is completely useless (by design).
		 * Use the static `create` method to create a `File` object.
        **/ 
        class REVSPACE_GAMEFILESYSTEM_API File : public std::enable_shared_from_this<File> {
		private:

			/**
			 * @brief Allow FileObjects to access the internal data.
			 **/
			friend class View;

			/**
			 * @brief Platform handle/index for the file itself.
			 **/
			PlatformFileHandle file_handle;

			/**
			 * @brief File path, provided by std::filesystem.
			 */
			std::filesystem::path path;

			/**
			 * @brief File Access mode.
			 */
			FileAccess access;

		private:
			/**
			 * @brief Internal, platform-specific, open code.
			 *
			 * Delete the file object if you no longer need it rather than
			 * hacking access to `open` and `closed`; messing with open state
			 * while it is mapped will lead to madness.
			 */
			bool open();

			/**
			 * @brief Internal, platform-specific, shutdown/deconstruction code.
			 *
			 * This is called by the deconstructor.
			 */
			void close();

        public:
			/**
			 * @brief Gets the path to the file object.
			 */
			std::filesystem::path get_path() const;

			/**
			 * @brief Gets a view from the mapping.
			 * @param[in] offset Offset in the file.
			 * @param[in] length Length of the requested map.
			 * @return std::shared_ptr<Object>; nullptr on failure.
			 *
			 * If `offset` is beyond the bounds of the file, the result will
			 * always be a nullptr.
			 *
			 * if `length` is zero (`0`), the whole file is mapped.
			 */
			ViewPointer get_view(StorageOffset offset, StorageSize length);

			/**
			 * @brief Gets the size of the underlying file object.
			 *
			 * At this time this uses std::filesystem to achieve its task;
			 * this could likely be updated to use platform specific code
			 * though the need for the performance gain here is questionable
			 * at best.
			 **/
			StorageSize get_size() const;

			/**
			 * @brief Explicit constructor (designed to be useless).
			 *
			 * Use `create` instead.
			 **/
			File();

			/**
			 * @brief Deconstructor.
			 *
			 * An explicit deconstruct is required to ensure platform objects
			 * are freed and all maps are properly torn down.
			 */
			~File();

			/**
			 * @brief Creates a `File` object.
			 *
			 * This is not a constructor as the creation of the code
			 */
			static FilePointer create(const std::filesystem::path& path,
				FileAccess access = FileAccess::Read);
        };
    }
}


#endif//REVERSINGSPACE_STORAGE_FILE_HPP