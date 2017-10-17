# C++ Game FileSystem

An easy to use, developer friendly, FileSystem library written in C++.  Abstracts platform code where possible, with a fallback to `std::filesystem` as a default (or by CMake option).

**Until version 0.1.0 the API for this library will be up the in air.  We are trying to nail down something useful, but not overly stressful to implement.**

## Changes

All notable changes to this project will be documented in `CHANGELOG.md`, which shall be based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/); this project shall adhere to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).  Prior to the first major version (1.0), API stability is not guaranteed.

## Licence

Apache 2.0 or MIT (your choice); see `COPYRIGHT` for full statement.

## Platform Support

As the wiki suggests, there is limited support.  The first target is Windows (as it has the most irritating filesystem code), but what follows should explain a few things.

### Windows

The minimum version of Windows is Windows 10.  [Windows 8 reaches the end of its mainstream support in January 2018](https://support.microsoft.com/en-us/help/13853/windows-lifecycle-fact-sheet), and since this library is being started in October 2017, there is no reason to support Windows 7 or Windows 8.  There are many people still using these versions of Windows, but the `std::filesystem` fallback should cover them (if there are any APIs that are actually too new, which at the time of writing appears not to be the case).

### macOS

The current version of macOS to target is unclear.  Most of the filesystem operations will be handled by `std::filesystem` until someone with an updated Mac can test compilation; most of the code will likely be boilerplate based on the POSIX implementation.

### Linux

Linux support is in a state of "to be advised".  Using fairly standard Linux (and/or POSIX) code should give fairly wide support.