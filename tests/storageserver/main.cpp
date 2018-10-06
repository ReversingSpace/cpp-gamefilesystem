// This is a test for the StorageServer component.

#include <ReversingSpace/GameFileSystem/StorageServer.hpp>
#include <ReversingSpace/GameFileSystem/PlatformFile.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>

// Declare this once.
using FileType = reversingspace::gfs::PlatformFile;

int main(int argc, char **argv) {
	// Specify userland as the current directory.
	const std::filesystem::path userland_root = std::filesystem::current_path() / "userland";

	// Notify output.
	std::cout << "Using path for userland: '" << userland_root.string().c_str() << "'" << std::endl;

	// Build a storage server.
	auto storage_server = reversingspace::gfs::StorageServer<FileType>::create(userland_root);

	if (storage_server == nullptr) {
		std::cout << "Failed to create storage server instance." << std::endl;
		throw std::runtime_error("failed to construct storage server");
	}

	auto userland_dir = storage_server->get_userland();
	if (userland_dir == nullptr) {
		std::cout << "Userland is nullptr.  This shouldn't happen." << std::endl;
		throw std::runtime_error("userland is nullptr");
	}

	{
		const std::string test_filename_0 = "userland_file_0.ext";
		std::string test_string = "This is a simple test to confirm read/write works for the storage server and platform file type.";
		std::uint32_t test_string_length = test_string.length();
		const std::uint32_t test_string_length_size = sizeof(decltype(test_string_length));
		{
			auto test_file_0 = storage_server->get_userland_file(test_filename_0,
				reversingspace::storage::FileAccess::ReadWrite);
			if (test_file_0 == nullptr) {
				std::cout << "test_file_0 is nullptr.  This really shouldn't happen." << std::endl;
				throw std::runtime_error("test_file_0 is nullptr(rw)");
			}
			if (test_file_0->write((char*)&test_string_length, test_string_length_size) != test_string_length_size) {
				throw std::runtime_error("failed to write test string (length).");
			}
			if (test_file_0->write(test_string.data(), test_string_length) != test_string_length) {
				throw std::runtime_error("failed to write test string (data).");
			}
		}

		// Re-open
		{
			auto test_file_0 = storage_server->get_userland_file(test_filename_0,
				reversingspace::storage::FileAccess::Read);
			if (test_file_0 == nullptr) {
				std::cout << "test_file_0 is nullptr (on read).  This really shouldn't happen." << std::endl;
				throw std::runtime_error("test_file_0 is nullptr(r)");
			}
			test_string_length = 0;

			if (test_file_0->read((char*)&test_string_length, test_string_length_size) != test_string_length_size) {
				throw std::runtime_error("failed to read test string (length).");
			}

			// This is not how I'd choose to do it, but it's how you're
			// meant to do it (apparently).  char* buffers are also supported
			// but it's irrelevant as internally they are both just memcpy.

			// Pass in an empty vector and it resizes (magic!)
			std::vector<std::uint8_t> buffer;
			if (test_file_0->read(buffer, test_string_length) != test_string_length) {
				throw std::runtime_error("failed to read test string (data).");
			}

			// Make it a string, then test it.
			std::string test_string_readback(buffer.begin(), buffer.end());
			if (test_string != test_string_readback) {
				throw std::runtime_error("test string read back incorrectly.");
			}
		}
	}

	const std::filesystem::path tdl0_fs_path = std::filesystem::current_path() / "test_files";
	const std::filesystem::path tdl1_fs_path = std::filesystem::current_path() / "test_files2";
	{
		// Append directories to the dataland stack.
		{
			std::filesystem::create_directories(tdl0_fs_path);
			if (!std::filesystem::is_directory(tdl0_fs_path)) {
				std::cout << "Failed to create test look-up directory #1: " << tdl0_fs_path.string().c_str() << std::endl;
				throw std::runtime_error("test directory 1 failed to mount.");
			}

			reversingspace::gfs::DirectoryPointer<FileType> dir = std::make_shared<reversingspace::gfs::Directory<FileType>>(tdl0_fs_path);
			storage_server->mount(dir);
		}
		{
			std::filesystem::create_directories(tdl1_fs_path);
			if (!std::filesystem::is_directory(tdl1_fs_path)) {
				std::cout << "Failed to create test look-up directory #2: " << tdl1_fs_path.string().c_str() << std::endl;
				throw std::runtime_error("test directory 2 failed to mount.");
			}
			reversingspace::gfs::DirectoryPointer<FileType> dir = std::make_shared<reversingspace::gfs::Directory<FileType>>(tdl1_fs_path);
			storage_server->mount(dir);
		}


		// Create a few files to do test look-ups on.
		// Use fstream to do this, as it's not part of the core system and
		// can't be misread as 'cheating'.
		const char tf0_a[8] = "tf0tsta";
		const char tf0_0[8] = "tf0tst0";
		const char tf0_1[8] = "tf0tst1";
		const char tf1_1[8] = "tf1tst1";

		// Make sure to null terminate or it'll screw things up.
		// String I/O is great like this.
		{
			// Only in 0
			std::ofstream strm(tdl0_fs_path / "test_file_0a");
			strm << tf0_a;
			strm.put(0); // It doesn't null terminate.   Yeah, I know.
			strm.flush();
		}
		{
			std::ofstream strm(tdl0_fs_path / "test_file_0");
			strm << tf0_0;
			strm.put(0); // It doesn't null terminate.   Yeah, I know.
			strm.flush();
		}
		{
			std::ofstream strm(tdl1_fs_path / "test_file_0");
			strm << tf0_1;
			strm.put(0); // It doesn't null terminate.   Yeah, I know.
			strm.flush();
		}
		{
			std::ofstream strm(tdl1_fs_path / "test_file_1");
			strm << tf1_1;
			strm.put(0); // It doesn't null terminate.   Yeah, I know.
			strm.flush();
		}

		{
			auto tf0 = storage_server->get_file("test_file_0");
			char test[8];
			if (tf0->read(test, 8) != 8) {
				std::cout << "failed to read tf0" << std::endl;
				throw std::runtime_error("failed to read tf0");
			}
			if (strcmp(test, tf0_1) != 0) {
				// Encapsulate in std::string to prevent lack of null terminator from blowing things up.
				std::cout << "tf0 read back is invalid (should be `" << tf0_1 << "` but is `" << std::string(test) << "`" << std::endl;
				throw std::runtime_error("failed to read tf0");
			}
		}

		{
			auto tf1 = storage_server->get_file("test_file_1");
			char test[8];
			if (tf1->read(test, 8) != 8) {
				std::cout << "failed to read tf1" << std::endl;
				throw std::runtime_error("failed to read tf1");
			}
			if (strcmp(test, tf1_1) != 0) {
				// Encapsulate in std::string to prevent lack of null terminator from blowing things up.
				std::cout << "tf1 read back is invalid (should be `" << tf1_1 << "` but is `" << std::string(test) << "`" << std::endl;
				throw std::runtime_error("failed to read tf0");
			}
		}

		{
			auto tf = storage_server->get_file("test_file_0a");
			char test[8];
			if (tf->read(test, 8) != 8) {
				std::cout << "failed to read tfa" << std::endl;
				throw std::runtime_error("failed to read tfa");
			}
			if (strcmp(test, tf0_a) != 0) {
				// Encapsulate in std::string to prevent lack of null terminator from blowing things up.
				std::cout << "tf1 read back is invalid (should be `" << tf0_a << "` but is `" << std::string(test) << "`" << std::endl;
				throw std::runtime_error("failed to read tf0");
			}
		}

	}
	std::filesystem::remove_all(tdl0_fs_path);
	std::filesystem::remove_all(tdl1_fs_path);
	return 0;
}