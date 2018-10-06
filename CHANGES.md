# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

Dates are given based on Coordinated Universal Time (UTC).

## [0.0.2-fix0] - 2018-10-07

Enhancement to search path code (allowing removal).

### Added
- Adding `unregister_directory` to `ArchiveSystem`;
- Adding `unmount` to `StorageServer`;
- Added `std::filesystem::path get_path() const` to `gfs::FileSystem` (to enable non-pointer based testing):
  - Added to `Directory` and `Archive` to retain compatibility (in the case of `Directory` this is just a drop of `inline` status);
  - Added to `StorageServer` to fix its inheritance (it now returns the path of `userland`, or `""` if userland is a nullptr) -- this enables chaining of `StorageSystem` objects if required (though I still wouldn't recommend it).
- Added `std::filesystem::path get_path() const` to `storage::File` (for consistency and to enable `Archive` work to be kept clean).

### Changed
- Updated `ArchiveSystem` and `StorageServer` tests to include removal of mounts;
- Updated `ArchiveSystem` test's `MyArchive` implementation to reflect changes to `Archive` (from `FileSystem`).

## [0.0.2] - 2018-10-06

With the core code stable (or stable enough?), this is a push towards making things more stable and consistent.  The next step is `Archive` support, which is more or less exactly what it sounds like.

No archive builder code is scheduled to be added to the core library, nor is it currently a consideration (as the methods for building archives vary too much).

### Added
- `get_child_path` to `gfs::Directory`, to allow archive loading to be done without needing to pull the stored file from underneath (though it's a bit excessive, so it's an inline) -- this is required to reduce the `ArchiveSystem` overhead without restricting it;
- Added `get_path` to `gfs::Directory` (to enable equality testing);
- Partial resolution code for [Issue `#9`](https://github.com/ReversingSpace/cpp-gamefilesystem/issues/9):
  - `StorageSize get_absolute_offset() const` to `storage::View` (as an inline);
  - `StorageSize get_file_offset() const` to `storage::View` (as an inline);
  - `StorageSize get_offset() const` (as an inline).
- Archive support:
  - `gfs::Archive` definition (it's an interface);
  - `gfs::ArchiveLoaderFunc` definition (it's just a `std::function`);
  - `gfs::ArchiveSystem` (trivial templated class for loading archives);
  - An archive loader test.

### Fixed
- Added `GameFilesystem/Core.hpp` to `CMakeLists.txt` (it was previously missing, though it has no real effect short of being missing from IDEs);
- Removed duplicate header include from `CMakeLists.txt` (again, no ill effects);
- Removed old test from `Directory::create` so it actually creates on missing;
- Bad API inclusion/removal from the previous chain of patching (now compiles after clean).

## [0.0.1-pre-alpha3-test-fix] - 2018-10-06

Fix to StorageServer templating and addition of a simple test.

### Added
- Storage Server test;
- `Directory` constructor (to bypass symlinking, creation of directories, and all other `create` related code);
- Added flush to all `PlatformFile` write functions (as the view does not auto-flush anymore);
- Added `get_stored_file` to `PlatformFile` (to allow proper memory mapping to occur for advanced usage).

### Fixed
- Removed erroneous template from StorageServer's static `create`;
- Issue in `StorageServer` constructor;
- `access` variable not being passed through userland lookups.

### Changed
- `StorageServer::create` now contains `create_if_missing` (default true) to create missing paths without needing user intervention;
- `Directory::create` now creates the missing directory when it attempts to load it.  This can be bypassed by using a manual construction of the directory (which can be done by calling the constructor).

## [0.0.1-pre-alpha2-cleanup] - 2018-10-06

This is a minor code update for cleanliness, designed to change compilation only (and not the main code).

### Added
- Static/Shared testing (as split types);
- CMake support to add tests a little easier (it mirrors a test into shared and static, depending on the build type);
- MSVC guard for 4251 to `PlatformFile` (`std::shared_mutex` needing a DLL interface).

### Fixed
- CMake `SOURCE_GROUP` code now appropriately reflects pathing (without the need for thirdparty code, which is probably still a better idea for complex solutions);
- Restored `BUILD_STATIC_LIBS` option.

## [0.0.1-pre-alpha2] - 2018-10-05

This is a restoration patch to bring back some old functionality now that the core code is stable (and it spews far fewer warnings).  It is also a huge patch for moving towards a stable filesystem.  This will require a fairly substantial review, and lead to a need for a lot of testing.

### Added
- `gfs` namespace for GameFileSystem (step towards actual functionality);
- `gfs::StorageServer` skeleton;
- `BUILD_SHARED_LIBS` build option;
- `PARENT_SCOPE` detection and export aliases;
- Fixed dated reference to `Object` (from `storage` namespace);
- Added `tell` and `get_file_offset` to `storage::File`;
- (Restored) `API.hpp` file with minor changes;
- Prepended API;
- Added `gfs::Directory` (placeholder);
- Added `gfs::Archive` (placeholder);
- Added `gfs::File` (interface);
- Added `gfs::PlatformFile` (`storage::File` wrapper);
- Added `gfs::FileSystem` (interface);
- Added `gfs::HashFunction` and `gfs::HashedIdentity` for hashing primitives;
- Added `GameFileSystem/Core.hpp` to contain forwards;
- Added MSVC suppression code for 4244 (memcpy is size_t) and 4251 (`std::shared_mutex`, `std::shared_ptr`, `std::vector` dynamic linking export issues).

### Changed
- Restructured source files (into `Storage` subdirectory, mostly).

### Removed
- Old `storage::Directory` alias/forward.

## [0.0.1-pre-alpha1-b] - 2018-10-04

Initial POSIX support.  Tested on Linux (ARM).

### Added
- Initial POSIX code (completed from skeletal) from `riki` (dropped my own licence to push it into the repository, as well as decoupling it from the `riki` storage system);
- Added `get_size()` to View API for use with the raw data pointer.

### Fixed
- Minor documentation bugs in View API;
- Missing documentation param fields for `calculate_allowance`.

## [0.0.1-pre-alpha1] - 2018-10-03

Yet another review pull, adding I/O to the `View` object and expanding on the testing (adding `-internals-rw`).

### Added
- Added `read`, `read_from`, `write` and `write_to` functionality for both raw pointers (char*, and a size value) and vectors (`std::vector<std::uint8_t>`);
- Added RW test.

### Fixed
- Fixed the copyright year in the MIT Licence document;
- Bad view calculation code (muddled from ORL and riki projects where one updates internally and the other functions similar to this).

### Changed
- Pre-existing internals test now removes the file it creates.

## [0.0.1-pre] - 2018-10-03

Another review pull.  Moving forward from a full rewrite (prompted by `std::filesystem` becoming viable).

### Added
- Dependency on `filesystem` and `memory` (for `std::filesystem` and `std::shared_ptr`);
- `View` internal structure (for review);
- `File` API;
- Various type aliases for convenience;
- Removed shared/dynamic library;
- Test (for pre-submission validation).

### Changed
- Rewrote the entire API for `File` so it is a memory mapping base;
- Changed `GameFileSystem` namespace to `storage`;
- Changed `ReversingSpace` namespace to `reversingspace`.
- Updated `CMakeLists.txt` to reflect changes;
- Lowered CMake version requirement to 3.0;
- Updated HEADER (and headers) to reflect 2018 being here.

### Internal TODO Markings
- Complete View code (read/write);
- Symlink consideration/discussion (`Issue #2`);
- Directory API;
- "FileSystem" layer (dependent sibling of `storage` namespace).

## [0.0.0-pre] - 2017-10-18
### Added
- This change log (`CHANGES.md`);
- README.md (`README.md`);
- Contributors list (`CONTRIBUTORS.md`);
- Maintainers list (`MAINTAINERS.md`);
- Project API definition (`API.hpp`);
- CMake build file;
- File definition (`File.hpp`);
- Skeletal files and directories to allow compilation passes for systems testing C++ validity (ignore them for now, will list them later).