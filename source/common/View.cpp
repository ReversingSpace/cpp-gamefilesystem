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

// This is the COMMON Object (view/map) file.
#include <ReversingSpace/Storage/File.hpp>

namespace reversingspace {
	namespace storage {
		StorageSize View::seek(StorageOffset offset, Seek whence) {
			std::unique_lock lock(rw_mutex);
			StorageOffset cur = cursor;
			switch (whence) {
				// 0 + offset
				case Seek::Set:
				{
					cur = offset;
				} break;

				// current + offset
				case Seek::Current:
				{
					cursor += offset;
				} break;

				// end + offset
				case Seek::End:
				{
					cur = view_length + offset;
				} break;
			}

			// Clamp
			if (offset < 0) {
				cur = 0;
			} else {
				if ((StorageSize)cur > view_length) {
					cur = view_length;
				}
			}
			cursor = cur;

			return cursor;
		}
	}
}