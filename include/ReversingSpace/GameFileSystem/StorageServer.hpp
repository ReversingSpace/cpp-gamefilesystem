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

#ifndef REVERSINGSPACE_GAMEFILESYSTEM_STORAGESERVER_HPP
#define REVERSINGSPACE_GAMEFILESYSTEM_STORAGESERVER_HPP

#include <ReversingSpace/GameFileSystem/Core.hpp>
#include <ReversingSpace/GameFileSystem/Directory.hpp>
#include <ReversingSpace/GameFileSystem/File.hpp>
#include <ReversingSpace/GameFileSystem/FileSystem.hpp>

namespace reversingspace {
	namespace gfs {
		/**
		 * @brief Simple storage server system.
		 *
		 * A StorageServer is a centralised access point for files.  It
		 * is designed to function as an abstraction layer above 'filesystem'
		 * type objects, and so leans on them directly.
		 */
		template<class UserlandFileType>
		class REVSPACE_GAMEFILESYSTEM_API StorageServer : public FileSystem {
		private:
			/**
			 * @brief 'Stack' of data mounts.
			 *
			 * This is a vector of read-only mounts.
			 *
			 * This is processed backwards (using a reverse iterator).
			 */
			std::vector<FileSystemPointer> dataland;

			/**
			 * @brief Userland storage space.
			 *
			 * Only one userland storage space is provided by this system.
			 */
			DirectoryPointer<UserlandFileType> userland;

			/**
			 * @brief Pointer to a hash function.
			 */
			HashFunction hash_function;

		public:
			/**
			 * @brief Constructs a storage server instance from a path.
			 * @param[in] userland_path  Path to valid userland.
			 * @param[in] hash_function  (Default) HashFunction to be used.
			 *
			 * This is a typical constructor.
			 */
			StorageServer(const std::filesystem::path& userland_path,
				HashFunction hash_function = nullptr) :
				hash_function(hash_function),
				userland(std::make_shared<Directory>(userland_path))
			{}

			/**
			 * @brief Virtual deconstructor.
			 */
			virtual ~StorageServer() {}

			/**
			 * @brief Gets the userland directory pointer.
			 * @return Pointer to userland.
			 */
			DirectoryPointer<UserlandFileType> get_userland() {
				return userland;
			}

			/**
			 * @brief Mounts a `FileSystemPointer` instance in dataland.
			 * @param[in] mountable Mountable fileSystem instance.
			 * @param[in] position Position into which to install it.
			 * @return true on success; false on failure.
			 *
			 * If the `position` argument is larger than the total stack space
			 * the value is pushed to the end.
			 *
			 * If the `mountable` argument is not valid (for the purposes of
			 * being used as a `FileSystemPointer` in this context), the
			 * insert will be aborted and false will be returned.
			 */
			bool mount(FileSystemPointer mountable, unsigned int position = -1) {
				if (mountable == nullptr) {
					return false;
				}
				if (position > dataland.size()) {
					dataland.push_back(mountable);
					return true;
				}
				dataland.insert(dataland.begin() + position, mountable);
				return true;
			}

			/**
			 * @brief Fetch a file from the dataland (only).
			 * @tparam Identity type to use in look-up.
			 * @param[in] identity to be used.
			 * @return  Pointer to a file (or nullptr on failure).
			 *
			 * If the underlying dataland mount does not support the lookup type
			 * it will fail, regardless of whether it exists there or not.
			 */
			virtual FilePointer get_dataland_file(HashedIdentity identity) {
				FilePointer file = nullptr;
				for (auto mount = dataland.rbegin(); mount != dataland.rend(); ++mount) {
					file = (*mount)->get_file(identity);
					if (file != nullptr) {
						break;
					}
				}
				return file;
			}

			/**
			 * @brief Fetch a file from the dataland (only).
			 * @tparam Identity type to use in look-up.
			 * @param[in] identity to be used.
			 * @return  Pointer to a file (or nullptr on failure).
			 *
			 * If the underlying dataland mount does not support the lookup type
			 * it will fail, regardless of whether it exists there or not.
			 */
			virtual FilePointer get_dataland_file(StringIdentity identity) {
				FilePointer file = nullptr;
				for (auto mount = dataland.rbegin(); mount != dataland.rend(); ++mount) {
					file = (*mount)->get_file(identity);
					if (file != nullptr) {
						return file;
					}
				}
				if (hash_function != nullptr) {
					auto hashed_identity = hash_function(identity);
					return get_dataland_file(hashed_identity);
				}
				return nullptr;
			}

			/**
			 * @brief Fetch a file from the userland (only).
			 * @tparam Identity type to use in look-up.
			 * @param[in] identity   Identity to be used.
			 * @param[in] access     Access to be used.
			 * @return  Pointer to a file (or nullptr on failure).
			 *
			 * If the underlying dataland mount does not support the lookup type
			 * it will fail, regardless of whether it exists there or not.
			 */
			virtual FilePointer get_userland_file(StringIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) {
				return userland->get_file(identity);
			}

		public: // FileSystem
			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 *
			 * This may be overloaded.
			 *
			 * This cannot get files from the userland due to its typing.
			 */
			virtual FilePointer get_file(HashedIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) {
				return get_dataland_file(identity);
			}

			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 *
			 * This may be overloaded.
			 *
			 * Userland lookup first, followed by dataland.
			 */
			virtual FilePointer get_file(StringIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) {
				auto file = get_userland_file(identity);
				if (file != nullptr) {
					return file;
				}
				file = get_dataland_file(identity);
				if (file != nullptr) {
					return file;
				}
				if (hash_function != nullptr) {
					auto hashed_identity = hash_function(identity);
					file = get_dataland_file(hashed_identity);
				}
				return file;
			}

		public: // Helper
			/**
			 * @brief Inline static helper to construct a StorageServer.
			 * @param[in] userland_path  Path to valid userland.
			 * @param[in] hash_function  (Default) HashFunction to be used.
			 * @return shared_ptr to a StorageServer, or nullptr on failure.
			 *
			 * This helper exists to ensure creation occurs only if
			 * the userland_path exists and is valid.
			 */
			template<typename UserlandFileType>
			inline static StorageServerPointer<UserlandFileType> create(const std::filesystem::path& userland_path,
				HashFunction hash_function = nullptr) {
				auto userland = userland_path;

				// Allow a symlink pointing to a valid directory.
				// I've seen some dumb things in my time, so compensate for people being insane.
				//
				// I know this isn't pretty, but it's worth doing for the rare cases where
				// it actually has a good reason (hello Raspberry Pi USB mounts).
				while (std::filesystem::is_symlink(userland)) {
					userland = std::filesystem::read_symlink(userland);
				}

				// Fail if it isn't a directory.
				if (!std::filesystem::is_directory(userland)) {
					return nullptr;
				}

				// Create the storage server.
				auto server = std::make_shared<StorageServer>(userland);
				server->hash_function = hash_function;
				return server;
			}
		};
	}
}

#endif//REVERSINGSPACE_GAMEFILESYSTEM_STORAGESERVER_HPP