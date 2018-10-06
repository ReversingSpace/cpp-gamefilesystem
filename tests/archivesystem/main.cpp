// Test for ReversingSpace/cpp-gamefilesystem.

#include <ReversingSpace/GameFileSystem.hpp>
#include <ReversingSpace/GameFileSystem/Archive.hpp>
#include <ReversingSpace/GameFileSystem/Directory.hpp>
#include <ReversingSpace/GameFileSystem/File.hpp>
#include <ReversingSpace/GameFileSystem/PlatformFile.hpp>
#include <ReversingSpace/Storage/File.hpp>

#include <iostream>
#include <stdexcept>

// memcmp, memcpy
#include <cstring>

using FileType = reversingspace::gfs::PlatformFile;

// Normally this wouldn't go here, but I need it for loading and building.
const char HEADER[4] = { 0,1,2,3 };

// This is re-used.
const char JUNK[16] = { 10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9 };

/// Trivial archive type.
class MyArchive: public reversingspace::gfs::Archive {
private:
	/// Internal pointer.
	reversingspace::storage::FilePointer file;

	/// Number of entries.
	std::uint32_t count;

public:
	/// Trivial constructor.
	MyArchive(reversingspace::storage::FilePointer file, std::uint32_t entries): file(file), count(entries),
		reversingspace::gfs::Archive() {}

public: // Archive
	std::uint32_t get_child_count() const {
		return count;
	}

public: // FileSystem
	reversingspace::gfs::FilePointer get_file(reversingspace::gfs::HashedIdentity identity,
		reversingspace::storage::FileAccess access) {
		return nullptr;
	}

	reversingspace::gfs::FilePointer get_file(reversingspace::gfs::StringIdentity identity,
		reversingspace::storage::FileAccess access) {
		return nullptr;
	}
};

reversingspace::gfs::ArchivePointer my_archive_loader(reversingspace::storage::FilePointer file) {
	auto header_view = file->get_view(0, 8);
	if (memcmp(header_view->get_data_pointer(), HEADER, 4) != 0) {
		return nullptr;
	}

	// Don't do this, it's bad.
	std::uint32_t count = 0;
	memcpy(&count, (char*)header_view->get_data_pointer() + 4, sizeof(decltype(count)));
	
	return std::make_shared<MyArchive>(file, count);
}

int main(int argc, char **argv) {
	// Create a userland directory for archive testing.
	auto userland_directory_path = std::filesystem::current_path() / "archive_test";
	auto userland_directory = reversingspace::gfs::Directory<FileType>::create(userland_directory_path);

	if (userland_directory == nullptr) {
		std::cout << "Failed to create or find userland testing directory." << std::endl;
		return 0;
	}

	// Make a bad archive.
	{
		// This is a platform file; just crash if it's bad.
		auto archive = userland_directory->get_file("bad_archive", reversingspace::storage::FileAccess::ReadWrite);
		archive->write((char*)JUNK, sizeof(JUNK));
	}

	// Make a good archive.
	{
		// This is a platform file; just crash if it's bad.
		auto archive = userland_directory->get_file("good_archive", reversingspace::storage::FileAccess::ReadWrite);
		
		// Header goes first.
		archive->write((char*)HEADER, sizeof(HEADER));

		// Count is just the number of bytes we're about to write.
		std::uint32_t count = 16;
		archive->write((char*)&count, sizeof(decltype(count)));

		// Write 16 bytes (just use junk here)
		archive->write((char*)JUNK, sizeof(JUNK));
	}

	{
		// Create the archive system.
		reversingspace::gfs::ArchiveSystem<reversingspace::gfs::Archive, FileType> archive_system;
		archive_system.register_loader(my_archive_loader);
		archive_system.register_directory(userland_directory);

		// Test bad
		auto bad_archive = archive_system.load("bad_archive");
		if (bad_archive != nullptr) {
			printf("bad archive loaded (this shouldn't happen)");
			return 1;
		}

		// Test good
		auto good_archive = archive_system.load("good_archive");
		if (good_archive == nullptr) {
			printf("good archive failed to load (this shouldn't happen)");
			return 1;
		}
	}

	std::filesystem::remove_all(userland_directory_path);

	return 0;
}