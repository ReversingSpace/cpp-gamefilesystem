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

// POSIX view file.

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
	namespace reversingspace {
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
				std::size_t real_size = view_length + ((u8*)view_pointer - data);
				::munmap(const_cast<u8*>(data), real_size);
				// Remove the reference here to trigger the shared_ptr deconstruction.
				this->file = nullptr;
			}
		}
	}
}
#endif//defined(_POSIX_VERSION)
#endif//defined(__unix__) || (defined (__APPLE__) && defined (__MACH__))