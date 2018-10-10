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

// This is the COMMON file code.

#include <ReversingSpace/Storage/File.hpp>

namespace reversingspace {
	namespace storage {

		// Construct with sane defaults where required;
		// otherwise rely on constructors (e.g. in the vector).
		File::File(): file_handle(PLATFORM_INVALID_FILE_HANDLE) {}

		File::~File() {
			close();
			
			// Probably not required, so commented out if issues do arise.
			//file_handle = PLATFORM_INVALID_FILE_HANDLE;
		}

		std::filesystem::path File::get_path() const {
			auto p = path;
			return p;
		}

		FilePointer File::create(const std::filesystem::path& path,
				FileAccess access) {

			// Partial block on updating:
			// Issue #2 (Symlink support)

			// Fail to create if no creation is passed in.
			if (((int)access & (int)FileAccess::Write)) {
				// Force creation for write.
				auto parent = path.parent_path();
				if (!std::filesystem::is_directory(parent)) {
					if (!std::filesystem::create_directories(parent)) {
						// Failed to create directory (and did not exist before).
						return nullptr;
					}
				}
			} else {
				// Doesn't exist, so can't read it.
				if (!std::filesystem::exists(path)) {
					return nullptr;
				}

				// Not a regular file, so fail.
				// Partial blocker on Issue #2
				if (!(std::filesystem::is_regular_file(path))) {
					return nullptr;
				}
			}

			// Create a default file instance.
			auto file = std::make_shared<File>();

			// Feed it the minimum requirements.
			file->path = path;
			file->access = access;
			
			// Use the platform-specific open.
			if (file->open()) {
				return file;
			}
			
			// Rely on std::shared_ptr to clean-up here.
			// Failure -> nullptr;
			return nullptr;
		}

		StorageSize File::get_size() const {
			return std::filesystem::file_size(path);
		}

		ViewPointer File::get_view(StorageOffset offset, StorageSize length) {
			ViewPointer view = std::make_shared<View>();
			view->file_offset = offset;
			view->view_length = length;
			view->file = shared_from_this();
			view->view_pointer = nullptr; // prevent bad deletion code.
			if (view->open_mapping()) {
				return view;
			}
			return nullptr;
		}
	}
}