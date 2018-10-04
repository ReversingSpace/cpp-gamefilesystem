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

// memcpy
#include <cstring>

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

		StorageSize View::calculate_allowance(StorageSize offset, StorageSize requested) {
			// I messed this up as the code came from a few projects which were
			// interlinked a little heavily.  The idea here is to detect the
			// requested amount (it used to handle the offset change in here);
			// so getting the change from `larger - smaller` is the first step.

			// Example data:
			// cursor:        150
			// requested:     100
			// view_length:   240
			//
			// offset is the cursor.
			//
			// This means we overshoot by 10, which should be the modifier.
			// The aim here is to get that request down by the modifier, so
			// it should return 90.

			// Safety.
			// This kills the request if the cursor/offset is >= the view_length.
			if (offset >= view_length) {
				return 0;
			}

			// This is the offset to check.  In the example,
			// new_offset = 150 + 100
			//            = 250
			StorageSize new_offset = offset + requested;

			// In the example this trips because 250 > 240.
			if (new_offset > view_length) {
				// 1. Get the modifier.
				//    250 - 240 = 10.
				//
				// So far, so good.
				auto modifier = (new_offset - view_length);

				// 2. Modify the request.
				//
				// new_requested = requested - modifier
				//               = 100 - 10
				//               = 90
				auto new_requested = requested - modifier;

				// Return the new_requested size.
				return new_requested;
			}
			return requested;
		}

		StorageSize View::read(char* data, StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto request = (size_t)calculate_allowance(cursor, requested);
			memcpy(data, (char*)view_pointer + cursor, request);
			cursor += request;
			return request;
		}

		StorageSize View::read(std::vector<std::uint8_t>& data, StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto request = (size_t)calculate_allowance(cursor, requested);
			if (data.size() < request) {
				data.resize(request);
			}
			memcpy(data.data(), (char*)view_pointer + cursor, request);
			cursor += request;
			return request;
		}

		StorageSize View::read_from(StorageOffset offset, char* data, StorageSize requested) {
			auto request = calculate_allowance(offset, requested);
			memcpy(data, (char*)view_pointer + offset, request);
			return request;
		}

		StorageSize View::read_from(StorageOffset offset, std::vector<std::uint8_t>& data, StorageSize requested) {
			auto request = calculate_allowance(offset, requested);
			if (data.size() < request) {
				data.resize(request);
			}
			memcpy(data.data(), (char*)view_pointer + offset, request);
			return request;
		}

		StorageSize View::write(char* data, StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto request = calculate_allowance(cursor, requested);
			memcpy((char*)view_pointer + cursor, data, request);
			cursor += request;
			return request;
		}

		StorageSize View::write(std::vector<std::uint8_t>& data, StorageSize requested) {
			std::unique_lock lock(rw_mutex);
			auto request = calculate_allowance(cursor, requested);
			if (data.size() < request) {
				data.resize(request);
			}
			memcpy((char*)view_pointer + cursor, data.data(), request);
			cursor += request;
			return request;
		}

		StorageSize View::write_to(StorageOffset offset, char* data, StorageSize requested) {
			auto request = calculate_allowance(offset, requested);
			memcpy((char*)view_pointer + offset, data, request);
			return request;
		}

		StorageSize View::write_to(StorageOffset offset, std::vector<std::uint8_t>& data, StorageSize requested) {
			auto request = calculate_allowance(offset, requested);
			if (data.size() < request) {
				data.resize(request);
			}
			memcpy((char*)view_pointer + offset, data.data(), request);
			return request;
		}
	}
}