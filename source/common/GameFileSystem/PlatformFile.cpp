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

#include <ReversingSpace/GameFileSystem/PlatformFile.hpp>
#include <ReversingSpace/Storage/File.hpp>

namespace reversingspace {
	namespace gfs {
		storage::StorageSize PlatformFile::seek(storage::StorageOffset offset,
			storage::Seek whence) {

			using namespace storage;

			auto file_size = stored_file->get_size();

			StorageOffset cur = cursor;
			switch (whence) {
				// 0 + offset
				case Seek::Set: {
					cur = offset;
				} break;

				// current + offset
				case Seek::Current: {
					cursor += offset;
				} break;

				// end + offset
				case Seek::End: {
					cur = file_size + offset;
				} break;
			}

			// Clamp
			if (offset < 0) {
				cur = 0;
			} else {
				if ((StorageSize)cur > file_size) {
					cur = file_size;
				}
			}
			cursor = cur;

			return cursor;
		}

		storage::StorageSize PlatformFile::read(char* data,
			storage::StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto view = stored_file->get_view(cursor, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->read(data, requested);
			cursor += count;
			return count;
		}

		storage::StorageSize PlatformFile::read(std::vector<std::uint8_t>& data,
			storage::StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto view = stored_file->get_view(cursor, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->read(data, requested);
			cursor += count;
			return count;
		}

		storage::StorageSize PlatformFile::read_from(storage::StorageOffset offset,
			char* data, storage::StorageSize requested) {
			std::shared_lock lock(rw_mutex);
			auto view = stored_file->get_view(offset, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->read(data, requested);
			return count;
		}

		storage::StorageSize PlatformFile::read_from(storage::StorageOffset offset,
			std::vector<std::uint8_t>& data,
			storage::StorageSize requested) {
			std::shared_lock lock(rw_mutex);
			auto view = stored_file->get_view(offset, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->read(data, requested);
			return count;
		}

		storage::StorageSize PlatformFile::write(char* data,
			storage::StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto view = stored_file->get_view(cursor, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->write(data, requested);
			cursor += count;
			view->flush();
			return count;
		}

		storage::StorageSize PlatformFile::write(std::vector<std::uint8_t>& data,
			storage::StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto view = stored_file->get_view(cursor, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->write(data, requested);
			cursor += count;
			view->flush();
			return count;
		}

		storage::StorageSize PlatformFile::write_to(storage::StorageOffset offset,
			char* data, storage::StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto view = stored_file->get_view(offset, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->write(data, requested);
			view->flush();
			return count;
		}

		storage::StorageSize PlatformFile::write_to(storage::StorageOffset offset,
			std::vector<std::uint8_t>& data,
			storage::StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto view = stored_file->get_view(offset, requested);
			if (view == nullptr) {
				return 0;
			}
			auto count = view->write(data, requested);
			view->flush();
			return count;
		}
	}
}