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

// A vector is used to store views.
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
		class View {
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
		};

        /**
         * @brief Object on a filesystem.
         * 
         * A File object represents what the standard calls a 'regular_file'.
		 *
		 * The constructor is not hidden, but is completely useless (by design).
		 * Use the static `create` method to create a `File` object.
        **/ 
        class File : public std::enable_shared_from_this<File> {
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