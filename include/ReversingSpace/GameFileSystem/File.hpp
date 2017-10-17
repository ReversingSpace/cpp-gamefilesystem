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

/**
 * @file File.hpp
 * @brief Virtual File definition.
**/

#ifndef __REVERSINGSPACE__GAMEFILESYSTEM__FILE_HPP__
#define __REVERSINGSPACE__GAMEFILESYSTEM__FILE_HPP__

// API
#include <ReversingSpace/GameFileSystem/API.hpp>

// uint8_t
#include <cstdint>

namespace ReversingSpace
{
    namespace GameFileSystem
    {
        /**
         * @brief Virtual representation of a File on the FileSystem.
         * 
         * A File object represents a file that may or may not exist in the
         * underlying FileSystem.  The object is not necessarily interested
         * in tracking the underlying state (and so does not have a watcher
         * associated unless one is set within it), nor does it necessary
         * have to be saved.
         * 
         * There are a limited number of pure virtual methods provided in File
         * to ensure that certain access (and support) can be given to
         * downstream applications.  The rest is handled using inline template 
		 * member functions.
         * 
         * API is defined in the event it gets expanded (to prevent issues);
         * the API is not expected to be used, but is preventatively given
         * (to prevent compilation failures in the event of error).
        **/ 
        class GAMEFILESYSTEM_API File
        {
        public:
			enum class SeekOffset : std::uint8_t
			{
				Start = 0,
				Current,
				End
			};

			/**
			 * @brief Set the read and write cursor to a given position.
			 * @param whence   Position from which to offset.
			 * @param offset   Number of bytes to offset.
			 * @return         Offset after modification (from start).
			 *
			 * Offset Values:
			 * - 0 = Start
			 * - 1 = Current ('Skip')
			 * - 2 = End
			 */
			virtual unsigned int Seek(SeekOffset whence, int offset) = 0;

            /**
             * @brief Read a number of bytes from a given stream.
             * @param count    Number of bytes to read.
             * @param buffer   Buffer (char*) into which to read.
             * @return         Number of bytes read.
            **/ 
            virtual unsigned int ReadBytes(unsigned int count,
                char* buffer
            ) = 0;

            /**
             * @brief Write a number of bytes to a given stream.
             * @param count   Number of bytes to write.
             * @param buffer  Buffer (char*) from which to write.
             * @return        Number of bytes written.
            **/
            virtual unsigned int WriteBytes(unsigned int count,
                char* buffer
            ) = 0;

			/**
			 * @brief Read a value from a given stream.
			 * @tparam T       Type of value to be read.
			 * @param  stream  Stream from which to read the value.
			 * @param  value   Reference to the value.
			 * @return         true if successful; false otherwise.			 
			**/
			template<typename T>
			inline bool Read(std::istream &stream, T &value)
			{
				return ReadBytes(stream, sizeof(T), reinterpret_cast<char*>(&value)) == sizeof(T);
			}

			/**
			 * @brief Read one or more of a given type from a given stream.
			 * @tparam T       Type of value to be read.
			 * @param  stream  Stream from which to read the value.
			 * @param  count   Number of values to be read into the array.
			 * @param  values  Pointer to the array.
			 * @return         true if successful; false otherwise.			 
			**/
			template<typename T>
			inline bool ReadSome(std::istream &stream, unsigned int count, T *values)
			{
				if (count == 0)
				{
					return true;
				}
				auto size = sizeof(T) * count;
				return ReadBytes(stream, size, reinterpret_cast<char*>(&value)) == size;
			}

			/**
			 * @brief Write a value from a given stream.
			 * @tparam T       Type of value to be read.
			 * @param  stream  Stream to which to write the value.
			 * @param  value   Reference to the value.
			 * @return         true if successful; false otherwise.
			**/
			template<typename T>
			inline bool Write(std::ostream &stream, T &value)
			{
				return WriteBytes(stream, sizeof(T), reinterpret_cast<char*>(&value)) == sizeof(T);
			}

			/**
			 * @brief Write one or more of a value from a given stream.
			 * @tparam T       Type of value to be read.
			 * @param  stream  Stream to which to write the value.
			 * @param  count   Number of values to be read into the array.
			 * @param  values  Pointer to the array.
			 * @return         true if successful; false otherwise.
			**/
			template<typename T>
			inline bool WriteSome(std::ostream &stream, unsigned int count, T* value)
			{
				if (count == 0)
				{
					return true;
				}
				auto size = sizeof(T) * count;
				return WriteBytes(stream, size, reinterpret_cast<char*>(&value)) == size;
			}
        };
    }
}


#endif//__REVERSINGSPACE__GAMEFILESYSTEM__FILE_HPP__