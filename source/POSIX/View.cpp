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

// This is the POSIX View code.
#if defined(__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
#if defined(_POSIX_VERSION)

#include <ReversingSpace/Storage/File.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

namespace reversingspace {
	namespace storage {
		bool View::open_mapping() {
			// Typically page size.
			std::uint64_t granularity = GET_PLATFORM_GRANULARITY();

			// File size (for later use)
			std::uint64_t file_size = (std::uint64_t)file->get_size();

			// Size of mapping is either length, or, if it's zero, 
			// then map the whole file.  If it's not, map the given length.
			std::uint64_t mapping_size = (view_length == 0)
				? file_size - (std::uint64_t)file_offset
				: view_length;

			// The real offset is calculated by page size, this is the
			// number of pages into a file.
			std::uint64_t real_offset = (file_offset / granularity) * granularity;

			// Offset + Length
			std::uint64_t offset_plus_size = file_offset + view_length;

			if ((int)file->access & (int)FileAccess::Write) {
				if (file_offset + mapping_size > file_size) {
					// Need to truncate *up* otherwise the map will fail.
					if (ftruncate(file->file_handle, file_offset + mapping_size) == -1) {
						return false;
					}
					file_size = file_offset + mapping_size;
				}
			}

			int prot = 0;
			int mapping = MAP_SHARED; // 0;
			switch (file->access) {
				case FileAccess::Read: {
					prot = PROT_READ;
					//mapping = MAP_PRIVATE | MAP_POPULATE;
				} break;
				case FileAccess::Write: {
					// 'read' to ensure cursor work doesn't ever break it.
					prot = PROT_READ | PROT_WRITE;
					//mapping = MAP_SHARED;
				} break;
				case FileAccess::ReadWrite: {
					prot = PROT_READ | PROT_WRITE;
					//mapping = MAP_SHARED;
				} break;
				case FileAccess::ReadExecute: {
					prot = PROT_READ | PROT_EXEC;
					//mapping = MAP_PRIVATE | MAP_POPULATE;
				} break;
				case FileAccess::Execute: {
					prot = PROT_READ | PROT_EXEC;
					//mapping = MAP_PRIVATE | MAP_POPULATE;
				} break;
				case FileAccess::ReadWriteExecute: {
					prot = PROT_READ | PROT_WRITE | PROT_EXEC;
					//mapping = MAP_SHARED;
				} break;
			}

			view_pointer = ::mmap(
				0,
				file_offset - real_offset + mapping_size,
				prot,
				mapping,
				file->file_handle,
				real_offset
			);
			if (view_pointer == MAP_FAILED) {
				return false;
			}
			cursor = 0;
			view_pointer = (char*)view_pointer + file_offset - real_offset;
			return true;
		}

		bool View::flush() {
			std::uint64_t granularity = GET_PLATFORM_GRANULARITY();
			char* data = (char*)view_pointer - (
				file_offset - ((file_offset / granularity) * granularity)
			);
			std::size_t mapped_size = view_length + ((char*)view_pointer - data);
			auto result = ::msync(data, mapped_size, MS_SYNC);
			return result == 0;
		}

		View::~View() {
			if (file != nullptr) {
				// Flush (if appropriate)
				if ((int)file->access & (int)FileAccess::Write) {
					flush();
				}
			}

			// Unmap
			if (view_pointer != nullptr) {
				std::uint64_t granularity = GET_PLATFORM_GRANULARITY();
				char* data = (char*)view_pointer - (
					file_offset - (
					(file_offset / granularity) * granularity)
				);
				std::size_t real_size = view_length + ((char*)view_pointer - data);
				::munmap(data, real_size);
				// Remove the reference here to trigger the shared_ptr deconstruction.
				this->file = nullptr;
			}
		}
	}
}
#endif//defined(_POSIX_VERSION)
#endif//defined(__unix__) || (defined (__APPLE__) && defined (__MACH__))