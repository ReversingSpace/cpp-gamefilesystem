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

#ifndef REVERSINGSPACE_GAMEFILESYSTEM_ARCHIVE_HPP
#define REVERSINGSPACE_GAMEFILESYSTEM_ARCHIVE_HPP

#include <ReversingSpace/GameFileSystem/Directory.hpp>
#include <ReversingSpace/GameFileSystem/FileSystem.hpp>
#include <ReversingSpace/Storage/Core.hpp>
#include <ReversingSpace/Storage/File.hpp>

#include <functional>
#include <string>
#include <vector>

namespace reversingspace {
	namespace gfs {
		/**
		 * @brief Archive Interface (FileSystem Implementatoin)
		 *
		 * An archive is designed to be a simple collection of data.
		 * It may be represented by one or more files (typically one or two),
		 * and must contains a way to index the data within it.  It implements
		 * (or more correctly inherits) the `FileSystem` interface.
		 *
		 * The purpose of an archive within a game, tool, or mod's context, is
		 * providing a collection of (typically) read-only data to the various
		 * systems (and subsystems).  In essence, it is a fancy collection.
		 *
		 * An archive does not have to contain compression (though it may).
		 * An archive may or may not be loaded via gfs (or using direct calls);
		 * most of the concerns pertaining to mounting come from beyond the
		 * scope of this library.
		 *
		 * Note: if an archive implementation is hashed-only, it may opt to
		 * drop any incoming `StringIdentity` based requests.  While this is
		 * probably a bad idea for many applications (particularly mods and
		 * developer tooling), it may be useful for end-game products (should
		 * there be a reason to try to hide how the hashing is done).
		 */
		class Archive : public FileSystem {
		public: // Archive specific.
			/// Virtual deconstructor/destructor.
			virtual ~Archive() {}

			/**
			 * @brief Gets the number of contained file objects.
			 * @return Number of contained objects.
			 *
			 * This shall return zero if it is empty, giving the system an
			 * opportunity to drop it rather than adding it to the mounts.
			 */
			virtual std::uint32_t get_child_count() const = 0;

		public: // FileSystem

			/**
			 * @brief Gets the path to the Archive itself.
			 *
			 * Should the archive be nested within another archive the system
			 * using it should find a way to reflect it.  This path it not
			 * tested for validity within this library, so it may be abused
			 * as needed.
			 */
			virtual std::filesystem::path get_path() const = 0;

			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 */
			virtual FilePointer get_file(HashedIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) = 0;

			/**
			 * @brief Gets a file from the underlying filesystem.
			 * @param[in] identity  Hashed identity of the file.
			 * @param[in] access    Access type (defaulting to `Read` state).
			 * @return Pointer to a file, or nullptr if look-up fails.
			 */
			virtual FilePointer get_file(StringIdentity identity,
				storage::FileAccess access = storage::FileAccess::Read) = 0;
		};

		/**
		 * @brief Loader of a given Archive Type.
		 *
		 * @param[in] file   Shared pointer to a `storage::File`.
		 * @return           Shared pointer to archive; nullptr on failure.
		 *
		 * This greatly simplifies the loading of archived data, converting
		 * the `storage::File` extracted from the `ArchiveSystem` into a
		 * `gfs::FileSystem` implementing type (i.e `Archive`).
		 */
		using ArchiveLoaderFunc = std::function<ArchivePointer(const storage::FilePointer file)>;

		/**
		 * @brief System for loading/finding archives.
		 *
		 * Archives need to be loaded, and forcing a particular pattern
		 * to the loading of data files limits the potential of the system.
		 *
		 * Much like the mount system this treats the internal directory vector
		 * as a stack, iterating from end to beginning.
		 */
		template<typename ArchiveType = Archive, typename FileType = PlatformFile>
		class ArchiveSystem {
		private: // Directories
			/// Directories used for look-up.
			std::vector<DirectoryPointer<PlatformFile>> directories;

			/// Loader functions.
			std::vector<ArchiveLoaderFunc> loaders;

		public: 
			/**
			 * @brief Constructs a default loader instance.
			 *
			 * This is explicitly written out to prevent confusion;
			 * nothing significant happens here.
			 */
			ArchiveSystem() = default;

			/**
			 * @brief Default deconstructor (nothing to do here)
			 */
			~ArchiveSystem() {}

			/**
			 * @brief Registers a directory as a look-up path.
			 * @param[in] dir  Shared pointer to a directory.
			 */
			void register_directory(DirectoryPointer<FileType> directory) {
				if (directory == nullptr) {
					return;
				}
				auto path = directory->get_path();
				for (auto dir = directories.begin(); dir != directories.end(); ++dir) {
					if ((*dir)->get_path() == path) {
						return;
					}
				}
				directories.push_back(directory);
			}

			/**
			 * @brief Unregisters/removes a Directory from the look-up path.
			 * @param[in] dir  Shared pointer to a directory.
			 */
			void unregister_directory(DirectoryPointer<FileType> directory) {
				auto path = directory->get_path();
				unregister_directory(path);
			}

			/**
			 * @brief Unregisters/removes a Directory from the look-up path.
			 * @param[in] path   Filesystem path of directory.
			 */
			void unregister_directory(const std::filesystem::path& path) {
				for (auto dir = directories.begin(); dir != directories.end(); ++dir) {
					if ((*dir)->get_path() == path) {
						directories.erase(dir);
						return;
					}
				}
			}

			/**
			 * @brief Adds a loader to the system.
			 * @param[in] loader   Loader function.
			 */
			void register_loader(ArchiveLoaderFunc loader) {
				loaders.push_back(loader);
			}

			/**
			 * @brief Attempts to load an archive.
			 * @param[in] name   Name of an archive.
			 * @return Shared pointer to an archive, or nullptr on failure.
			 */
			ArchivePointer load(const std::string& name) {
				// Step directories (top to bottom).
				for (auto dir = directories.rbegin(); dir != directories.rend(); ++dir) {
					auto path = (*dir)->get_child_path(name);

					// Read symlinks.
					while (std::filesystem::is_symlink(path)) {
						path = std::filesystem::read_symlink(path);
					}

					// Attempt a file load, this does an internal "is it valid" check.
					auto storage_file = storage::File::create(path);
					if (storage_file == nullptr) {
						continue;
					}

					// Find a valid handler.
					for (auto handler = loaders.begin(); handler != loaders.end(); ++handler) {
						auto archive = (*handler)(storage_file);
						if (archive != nullptr) {
							// Success.
							return archive;
						}
					}
				}
				// Failure to find or load.
				return nullptr;
			}
		};
	}
}

#endif//REVERSINGSPACE_GAMEFILESYSTEM_ARCHIVE_HPP
