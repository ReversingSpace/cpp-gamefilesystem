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

// This is the WINDOWS file code.
#if defined(_WIN32) || defined(_WIN64)

#include <ReversingSpace/Storage/File.hpp>
#include <Windows.h>

#if defined(_DEBUG)
#include <stdio.h>
#endif

namespace reversingspace {
#if REVERSINGSPACE_STORAGE_GRANULARITY == 1
	namespace platform {
		std::uint64_t get_granularity() {
			SYSTEM_INFO SystemInfo;
			GetSystemInfo(&SystemInfo);
			std::uint64_t granularity = SystemInfo.dwAllocationGranularity;
			return granularity;
		}
	}
#endif
	namespace storage {
		bool File::open() {

			// Shared mode is *always* READ|WRITE, as we don't care
			// and shouldn't care.  For games, this is a non-issue,
			// and it allows modders to do things.  Changing this
			// reflects a massive change that we probably don't want
			// to have to deal with.
			DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

			DWORD dwDesiredAccess = GENERIC_READ;
			DWORD dwCreationDisposition = OPEN_ALWAYS;
			DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
			
			switch (access) {
				case FileAccess::Read:
				{
					dwCreationDisposition = OPEN_EXISTING;
					dwFlagsAndAttributes = FILE_ATTRIBUTE_READONLY;
				} break;
				case FileAccess::Write: {
					dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
				} break;
				case FileAccess::ReadWrite: {
					dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
				} break;
				case FileAccess::ReadExecute:
				case FileAccess::Execute: {
					// This bodes terribly.
					dwDesiredAccess = GENERIC_READ | GENERIC_EXECUTE;
				} break;
				case FileAccess::ReadWriteExecute: {
					dwDesiredAccess = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE;
				} break;
			}

			// This creates (or opens) the file.  Naming is somewhat obvious,
			// but the arguments are kind of weird to anyone used to `fopen`.
			file_handle = ::CreateFileW(
				path.wstring().c_str(),
				dwDesiredAccess,
				dwShareMode,
				NULL,
				dwCreationDisposition,
				dwFlagsAndAttributes,
				NULL
			);

			if (file_handle == INVALID_HANDLE_VALUE) {
#if defined(_DEBUG)
				auto error = GetLastError();
				fprintf(stderr, "Error in CreateFileW call: %d\n", error);
#endif
				return false;
			}
			return true;
		}

		// Platform-specific terminate.
		void File::close() {
			::CloseHandle(file_handle);
		}
	}
}
#endif//defined(_WIN32) || defined(_WIN64)