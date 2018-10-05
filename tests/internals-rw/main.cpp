// Test for ReversingSpace/cpp-gamefilesystem.

#include <ReversingSpace/GameFileSystem.hpp>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

const int VIEW_SIZE = 4096;

int main(int argc, char **argv) {
	auto cwd = std::filesystem::current_path();
	auto test0 = cwd / "test-rw0.ext";
	auto test1 = cwd / "test-rw1.ext";

	// Should fail, by design.
	if (reversingspace::storage::File::create(test0) != nullptr) {
		throw std::runtime_error("why did you create test-rw0.ext?");
	}

	{
		// Be kind.
		int random_offset = rand() % VIEW_SIZE;
		if (random_offset + 64 > VIEW_SIZE) {
			random_offset -= 64;
		}

		std::uint64_t random_value = (std::uint64_t)(rand() % 0xFFFFFFFF) << 32
			| (std::uint64_t)(rand() % 0xFFFFFFFF);

		// This is just a dummy value.
		std::uint64_t random_value2 = 0;

		// Make it invalid for reading or writing.
		std::uint64_t random_value2_offset = VIEW_SIZE -
			(sizeof(decltype(random_value2)) / 2);

		auto test1_file = reversingspace::storage::File::create(test1,
			reversingspace::storage::FileAccess::ReadWrite);
		if (test1_file == nullptr) {
			throw std::runtime_error("test1_file is nullptr.");
		}
		std::string test_string_data = "This is a test.";
		std::uint32_t test_string_length = 0;
		const std::uint32_t test_string_length_size = sizeof(decltype(test_string_length));
		{
			auto view = test1_file->get_view(0, 4096);
			std::string test_string(test_string_data);
			test_string_length = test_string.length();
			if (view->write((char*)&test_string_length, test_string_length_size) != test_string_length_size) {
				throw std::runtime_error("failed to write test string (length).");
			}
			if (view->write(test_string.data(), test_string_length) != test_string_length) {
				throw std::runtime_error("failed to write test string (data).");
			}
			view->flush();
		}
		// view is gone
		{
			test_string_length = 0;
			auto file_size = test1_file->get_size();
			if (file_size  != 4096) {
				throw std::runtime_error("test1: should be 4096, but is " + file_size);
			}
			auto view = test1_file->get_view(0, 4096);
			if(view->read((char*)&test_string_length, test_string_length_size) != test_string_length_size) {
				throw std::runtime_error("failed to read test string (length).");
			}

			// This is not how I'd choose to do it, but it's how you're
			// meant to do it (apparently).  char* buffers are also supported
			// but it's irrelevant as internally they are both just memcpy.

			// Pass in an empty vector and it resizes (magic!)
			std::vector<std::uint8_t> buffer;
			if (view->read(buffer, test_string_length) != test_string_length) {
				throw std::runtime_error("failed to read test string (data).");
			}

			// Make it a string, then test it.
			std::string test_string(buffer.begin(), buffer.end());
			if (test_string_data != test_string) {
				throw std::runtime_error("test string read back incorrectly.");
			}
		}

		// Do random offset work here.
		{
			auto view = test1_file->get_view(0, 4096);
			if (view->write_to(random_offset, (char*)&random_value, sizeof(decltype(random_value))) != sizeof(decltype(random_value))) {
				throw std::runtime_error("failed to write random value.");
			}
			view->flush();

			auto allowance = view->calculate_allowance(
				random_value2_offset, sizeof(decltype(random_value2))
			);
			if (!(allowance == (sizeof(decltype(random_value2)) / 2))) {
				printf("[-] allowance test failed.\n");
			}
		}
		{
			auto view = test1_file->get_view(0, 4096);
			decltype(random_value) random_value_holder = 0;
			if (view->read_from(random_offset, (char*)&random_value_holder, sizeof(decltype(random_value))) != sizeof(decltype(random_value))) {
				throw std::runtime_error("failed to read random value.");
			}
			if (random_value_holder != random_value) {
				throw std::runtime_error("random value restored incorrectly; invalid read.");
			}
		}
	}
	// TODO: Should this be in the API?
	std::filesystem::remove(test1);
	return 0;
}