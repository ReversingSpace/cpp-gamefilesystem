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
		// Platform-specific terminate.
		void File::close() {
			::close(file_handle);
		}
	}
}

#endif//defined(_POSIX_VERSION)
#endif//defined(__unix__) || (defined (__APPLE__) && defined (__MACH__))