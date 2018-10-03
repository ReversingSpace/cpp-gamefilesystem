// Test for ReversingSpace/cpp-gamefilesystem.

#include <ReversingSpace/GameFileSystem.hpp>
#include <stdexcept>

int main(int argc, char **argv) {
	auto cwd = std::filesystem::current_path();
	auto test0 = cwd / "test0.ext";
	auto test1 = cwd / "test1.ext";

	// test 0
	// Should fail, by design.
	{
		if (reversingspace::storage::File::create(test0) != nullptr) {
			throw std::runtime_error("why did you create test0.ext?");
		}
	}

	// test 1
	{
		auto test1_file = reversingspace::storage::File::create(test1,
			reversingspace::storage::FileAccess::ReadWrite);

		{
			auto view = test1_file->get_view(0, 4096);
			view->flush();
		}
		// view is gone
		{
			auto file_size = test1_file->get_size();
			if (file_size  != 4096) {
				throw std::runtime_error("test1.ext should be 4096, but is " + file_size);
			}
		}

		{
			auto view = test1_file->get_view(4096, 8192);
			view->flush();
		}
		// view is gone
		{
			auto file_size = test1_file->get_size();
			if (file_size != 12288) {
				throw std::runtime_error("test1.ext should be 12288, but is " + file_size);
			}
		}
	}
	std::filesystem::remove(test1);

	return 0;
}