# C++ Game FileSystem

An easy to use, developer friendly, FileSystem library written in C++.  Due to the stabilisation of `std::filesystem`, this now uses `std::filesystem` everywhere it can, falling back to platform code only when absolutely required.

This was originally designed as a game virtual filesystem, and has since evolved to underpin such a thing by using memory mapping.  The core `filesystem` component is yet to be finalised and layered.

**Until version 0.1.0 the API for this library will be up the in air.  We are trying to nail down something useful, but not overly stressful to implement.**

## Changes

All notable changes to this project will be documented in `CHANGELOG.md`, which shall be based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/); this project shall adhere to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).  Prior to the first major version (1.0), API stability is not guaranteed.

## Licence

Apache 2.0 or MIT (your choice); see `COPYRIGHT` for full statement.

## Platform Support

As the wiki suggests, there is limited support.  The first target is Windows (as it has the most irritating filesystem code), but what follows should explain a few things.

**gcc 8 or better is required for `std::filesystem`.**

### Windows

The minimum version of Windows is Windows 10.  [Windows 8 reached the end of its mainstream support in January 2018](https://support.microsoft.com/en-us/help/13853/windows-lifecycle-fact-sheet), and since this library is being started in October 2017, there is no reason to support Windows 7 or Windows 8.

This code likely operates fine on these platforms, but testing becomes more difficult as people move off them.

### macOS

**Presently untested**

Support for Darwin/macOS (formerly Mac OS X) comes from POSIX support and `std::filesystem`.  It should be functional, but will require someone with an updated Mac to test it further.

**Getting std::filesystem and std::shared_mutex to function on macOS is a known challenge.**

### Linux

Linux functionality has been lightly tested for reading and writing using memory mapping.  Further testing is yet to come.

### OpenBSD

**Presently untested**

OpenBSD is currently untested, but documentation suggests it should be fine (provided you `pledge` appropriately).