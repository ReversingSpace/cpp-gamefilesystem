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

#ifndef REVERSINGSPACE_GAMEFILESYSTEM_DIRECTORY_HPP
#define REVERSINGSPACE_GAMEFILESYSTEM_DIRECTORY_HPP

#include <ReversingSpace/GameFileSystem/Core.hpp>
#include <ReversingSpace/GameFileSystem/FileSystem.hpp>
#include <ReversingSpace/Storage/Core.hpp>

namespace reversingspace {
	namespace gfs {
		/**
		 * @brief Directory Implementation.
		 */
		template<class FileType>
		class Directory : public FileSystem {
		private:
			/// Directory path.
			std::filesystem::path path;

		public:
			/**
			 * @brief Gets a child path from the directory.
			 * @param[in] name  Name of the child to be appended.
			 * @return child path (unchecked).
			 *
			 * This method does not check for validity of the path,
			 * this is a task left up to the user/developer.
			 *
			 * This is a helper/utility function for advanced use-cases
			 * where it's preferable to use the gfs's Directory API over
			 * direct `std::filesystem` interaction.
			 */
			inline std::filesystem::path get_child_path(const std::string& name) const {
				return path / name;
			}

			/**
			 * @brief Gets a copy of the internal path.
			 *
			 * This is a copy.
			 */
			inline std::filesystem::path get_path() const {
				auto p = path;
				return p;
			}

			/// Constructor.
			Directory() {}

			/**
			 * @brief Fallback constructor.
			 * @param[in] dirpath  Path to a directory.
			 */
			Directory(const std::filesystem::path &dirpath): path(dirpath) {}

			/**
			 * @brief Creates a directory.
			 * @param[in] dirpath  Path to a directory.
			 * @return Shared pointer to a Directory.
			 */
			static DirectoryPointer<FileType> create(const std::filesystem::path &dirpath) {
				std::filesystem::path dir = dirpath;
				
				while (std::filesystem::is_symlink(dir)) {
					dir = std::filesystem::read_symlink(dir);
				}

				// Fail if it isn't a directory.
				if (!std::filesystem::is_directory(dir)) {
					// Try to make it.
					if (!std::filesystem::create_directories(dir)) {
						return nullptr;
					}
				}
				auto directory = std::make_shared<Directory>();
				directory->path = dir;
				return directory;
			}

			/**
			 * @brief Creates a directory.
			 * @param[in] dirpath  Path to a directory.
			 * @return Shared pointer to a Directory.
			 */
			static DirectoryPointer<FileType> create(const std::string &dirpath) {
				return create(std::filesystem::path(dirpath));
			}

			/**
			 * @param Gets or creates a child directory.
			 * @param[in] dirpath  Name of the child directory.
			 * @return Shared pointer to a Directory, or nullptr on failure.
			 *
			 * The directory path may be multiple layers below.
			 */
			template<typename ChildFileType = FileType>
			DirectoryPointer<ChildFileType> child_directory(const std::string& dirpath) {
				auto child = path / dirpath;
				if (std::filesystem::is_directory(child)) {
					return create(child);
				}
				if (std::filesystem::create_directories(child)) {
					return create(child);
				}
				return nullptr;
			}

			/// Gets the number of child directories.
			size_t directory_count() {
				return (size_t)std::count_if(
					std::filesystem::directory_iterator(path),
					std::filesystem::directory_iterator(),
					static_cast<bool(*)(const std::filesystem::path&)>(std::filesystem::is_directory)
				);
			}

			/// Gets the number of child files.
			size_t file_count() {
				return (size_t)std::count_if(
					std::filesystem::directory_iterator(path),
					std::filesystem::directory_iterator(),
					static_cast<bool(*)(const std::filesystem::path&)>(std::filesystem::is_regular_file)
				);
			}

		public: // FileSystem
			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 *
			 * This will always fail due to the nature of the hashed identity.
			 */
			FilePointer get_file(HashedIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) {
				return nullptr;
			}

			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 */
			FilePointer get_file(StringIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) {
				return FileType::create(path / identity, access);
			}

			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @tparam OtherFileType  Other type of file to create/construct.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 */
			template<class OtherFileType>
			FilePointer get_file_typed(StringIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) {
				return OtherFileType::create(path / identity, access);
			}
		};
	}
}

#endif//REVERSINGSPACE_GAMEFILESYSTEM_DIRECTORY_HPP