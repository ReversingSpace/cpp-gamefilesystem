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

// This is the WINDOWS file code.
#if defined(__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h>
#if defined(_POSIX_VERSION)

#include <ReversingSpace/Storage/File.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

namespace reversingspace {
#if REVERSINGSPACE_STORAGE_GRANULARITY == 1
	namespace platform {
		std::uint64_t get_granularity() {
			// `getpagesize();` could be used, but portability concerns are
			// something worth considering due to the current test on inclusion.
			std::uint64_t granularity =
#if defined(_SC_PAGESIZE)
				sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
				sysconf(_SC_PAGE_SIZE);
#endif
			return granularity;
		}
	}
#endif

	namespace storage {
		bool File::open() {
			int flags = 0;
			mode_t mode = 0;

			const mode_t R_MODE = S_IRUSR | S_IRGRP | S_IROTH;
			const mode_t W_MODE = S_IWUSR;
			const mode_t E_MODE = S_IXUSR;

			// This needs more testing, as does the Win32 version.
			switch (access) {
				case FileAccess::Read: {
					flags = O_RDONLY;
					mode = R_MODE;
				} break;
				case FileAccess::Write: {
					// flags = O_WRONLY | O_CREAT;
					//  mode = O_RDWR;

					// Not all platforms are trustworthy here,
					// which, given this is POSIX, is weird.
					//
					// Read is required to prevent platforms that do a 'test read'
					// somewhere from freaking out.  This isn't ideal, but for
					// some reason cursor work freaks it out in some tests.
					flags = O_RDWR | O_CREAT;
					mode = R_MODE | W_MODE;
				} break;
				case FileAccess::ReadWrite: {
					flags = O_RDWR | O_CREAT;
					mode = R_MODE | W_MODE;
				} break;
				case FileAccess::ReadExecute: {
					flags = O_RDONLY;
					mode = R_MODE | E_MODE;
				} break;
				case FileAccess::Execute: {
					flags = O_RDONLY;
					// Enable read to prevent some weird edge cases in testing.
					mode = R_MODE | E_MODE;
				} break;
				case FileAccess::ReadWriteExecute: {
					flags = O_RDWR | O_CREAT;
					mode = R_MODE | E_MODE | W_MODE;
				} break;
			}

			file_handle = ::open(
				path.string().c_str(),
				flags,
				mode
			);
			return file_handle != PLATFORM_INVALID_FILE_HANDLE;
		}

		// Platform-specific terminate.
		void File::close() {
			::close(file_handle);
		}
	}
}

#endif//defined(_POSIX_VERSION)
#endif//defined(__unix__) || (defined (__APPLE__) && defined (__MACH__))