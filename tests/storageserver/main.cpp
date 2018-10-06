// This is a test for the StorageServer component.

#include <ReversingSpace/GameFileSystem/StorageServer.hpp>
#include <ReversingSpace/GameFileSystem/PlatformFile.hpp>

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv) {
	// Specify userland as the current directory.
	const std::filesystem::path userland_root = std::filesystem::current_path() / "userland";

	// Notify output.
	std::cout << "Using path for userland: '" << userland_root.string().c_str() << "'" << std::endl;

	// Build a storage server.
	auto storage_server = reversingspace::gfs::StorageServer<reversingspace::gfs::PlatformFile>::create(userland_root);

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

	return 0;
}