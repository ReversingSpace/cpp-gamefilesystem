# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

Dates are given based on Coordinated Universal Time (UTC).

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