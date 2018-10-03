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

// This is the Windows Object (view/map) file.
#if defined(_WIN32) || defined(_WIN64)

#include <ReversingSpace/Storage/File.hpp>
#include <Windows.h>

#include <stdio.h>

namespace reversingspace {
	namespace storage {
		bool View::open_mapping() {
			// Granluarity (typically the page size, though there are a few
			// super weird edge cases where this may not be true).
			std::uint64_t granularity = GET_PLATFORM_GRANULARITY();

			// Write mode is pulled from the file itself.
			int mapping_access = 0;
			int viewing_access = 0;
			switch (file->access) {
				case FileAccess::Read: {
					viewing_access = FILE_MAP_READ;
					mapping_access = PAGE_READONLY;
				} break;
				case FileAccess::Write:
				case FileAccess::ReadWrite: {
					viewing_access = FILE_MAP_WRITE;
					mapping_access = PAGE_READWRITE;
				} break;
				case FileAccess::Execute: {
					// This bodes terribly.
					viewing_access = FILE_MAP_EXECUTE;
					mapping_access = PAGE_EXECUTE;
				} break;
				case FileAccess::ReadExecute: {
					viewing_access = FILE_MAP_READ | FILE_MAP_EXECUTE;
					mapping_access = PAGE_EXECUTE_READ;
				} break;
				case FileAccess::ReadWriteExecute:  {
					viewing_access = FILE_MAP_ALL_ACCESS;
					mapping_access = PAGE_EXECUTE_READWRITE;
				} break;
			}

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

			// Windows API call
			file_map_handle = CreateFileMappingW(
				file->file_handle,
				0,
				mapping_access,
				offset_plus_size >> 32,
				offset_plus_size & 0xFFFFFFFF,
				0
			);
			if (file_map_handle == nullptr) {
#if defined(_DEBUG)
				auto error = GetLastError();
				fprintf(stderr, "failed to CreateFileMappingW: %d\n", error);
#endif
				return false;
			}

			// Get the data pointer.
			view_pointer = MapViewOfFile(
				file_map_handle,
				viewing_access,
				real_offset >> 32,
				real_offset & 0xFFFFFFFF,
				(SIZE_T)(file_offset - real_offset + mapping_size)
			);
			if (view_pointer == nullptr) {
#if defined(_DEBUG)
				auto error = GetLastError();
				fprintf(stderr, "failed to MapViewOfFile: %d\n", error);
#endif
				return false;
			}
			// Fix the view pointer.
			view_pointer = (char*)view_pointer + file_offset - real_offset;

			// Take no chances.
			cursor = 0;
			return true;
		}

		bool View::flush() {
			std::uint64_t granularity = GET_PLATFORM_GRANULARITY();
			char* data = (char*)view_pointer - (
				file_offset - (
					(file_offset / granularity) * granularity)
			);
			std::uint64_t mapped_size = view_length + ((char*)view_pointer - data);
			if (::FlushViewOfFile(data, (SIZE_T)mapped_size) == 0) {
				return false;
			}
			return FlushFileBuffers(file->file_handle) != 0;
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
				::UnmapViewOfFile(data);
			}

			::CloseHandle(file_map_handle);
			file_map_handle = PLATFORM_INVALID_FILE_HANDLE;
			// Remove the reference here to trigger the shared_ptr deconstruction.
			this->file = nullptr;
		}
	}
}
#endif//defined(_WIN32) || defined(_WIN64)