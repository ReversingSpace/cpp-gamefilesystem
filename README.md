# C++ Game FileSystem

This repository contains an easy to use, developer friendly, filesystem-like library written in modern C++.  The aim of this library is to supply indie games, tools, mods, and other applications with a mountable, flexible, and relatively performant (and relatively non-restrictive) filesystem.

By design, this library does not ship with any non-standard dependencies.  This means you can build it without needing to worry about weird upstream licenses, or other issues.  This just uses `std` code (including the STL), and platform specific calls (i.e. POSIX and Windows APIs at this time).

This was originally designed as a game virtual filesystem, and has since evolved to underpin such a thing by using memory mapping.  The core support stems from two parts:

 1. Memory mapping (`storage::File`) is used for the base files (and supplied in a way which allows for custom `gfs::Archive` creation -- see the `archivesystem` test for a very basic example);
 2. A FileSystem interface (`gfs::FileSystem`) for mounting types.  In this library this is:
   * `Directory` (a layered/wrapped directory) for standard 'on disk' work;
   * `Archive` (`gfs::Archive`) for non-standard collections (or standard ones, this library doesn't ship any dependencies);
   * `StorageServer` (which is, in itself, a FileSystem, so you can chain them if you really want -- useful for in-game tooling).

**Until version 0.1.0 the API for this library will be up the in air.  We are trying to nail down something useful, but not overly stressful to implement.**

## Changes

All notable changes to this project will be documented in `CHANGELOG.md`, which shall be based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/); this project shall adhere to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).  Prior to the first major version (1.0), API stability is not guaranteed.

## Licence

Apache 2.0 or MIT (your choice); see `COPYRIGHT` for full statement.

## Platform Support

**gcc 8 or better is required for `std::filesystem`.**

### Windows

The minimum version of Windows is Windows 10.  [Windows 8 reached the end of its mainstream support in January 2018](https://support.microsoft.com/en-us/help/13853/windows-lifecycle-fact-sheet), and since this library is being started in October 2017, there is no reason to support Windows 7 or Windows 8.

This code likely operates fine on these platforms, but testing becomes more difficult as people move off them.

### macOS

macOS should work if gcc 8 (or newer) is used.  `std::filesystem` and `std::shared_mutex` are the potential blockers here.

Confirming support would require someone with a modern Mac to test it.

### Linux

Linux support has been tested fairly heavily on ARM.  Since these are API calls, it should be fine on other architectures too.

### POSIX

Other POSIX systems (like OpenBSD) should be functional provided correct initialisation measures are taken (so on OpenBSD this is the right `pledge` call).