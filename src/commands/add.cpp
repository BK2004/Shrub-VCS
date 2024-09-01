#include "add.h"

namespace Commands {
	void Add::exec() {
		// Get path to staging folder
		std::filesystem::path staging_dir = get_svc_dir();
		if (staging_dir.empty()) {
			ERR(std::filesystem::current_path().string() << " is not being tracked.");
			SEE_HELP(init)
			return;
		}
		staging_dir = staging_dir / "staging";
		auto parent_dir = staging_dir.parent_path().parent_path();

		// There should be at least 1 arg for location
		if (this->parser->is_parsed()) {
			ERR("Usage: svc add <loc1> <loc2> ...")
			return;
		}

		auto copy_ops = std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive;
		while (!this->parser->is_parsed()) {
			std::string loc = this->parser->match_arg();

			if (!std::filesystem::exists(loc)) {
				std::cout << loc << " does not exist.\n";
				continue;
			}

			std::filesystem::path loc_path = std::filesystem::canonical(loc);
			if (parent_dir.string().size() > loc_path.string().size()) {
				std::cout << "Cannot add files outside of directory. (" << loc << ")\n";
				return;
			}

			// Ignore SVC_DIR
			if (loc_path == staging_dir.parent_path() || loc_path.filename() == ".git") continue;

			std::filesystem::path dir = loc_path.string().size() == parent_dir.string().size() ? staging_dir / "" : 
				staging_dir / loc_path.string().substr(parent_dir.string().size() + 1);

			if (std::filesystem::is_directory(loc_path)) {
				std::filesystem::create_directories(dir);
				for (auto& entry : std::filesystem::directory_iterator(loc_path)) {
					if (entry.path() == staging_dir.parent_path()  || entry.path().filename() == ".git") continue;
					std::filesystem::remove(dir / entry.path().filename());
					std::filesystem::copy(entry.path(), dir / entry.path().filename(), copy_ops);
				}
			} else {
				std::filesystem::remove(dir);
				std::filesystem::create_directories(dir.parent_path());
				std::filesystem::copy(loc_path, dir, copy_ops);
			}
		}
	}

	void Add::help() {
		std::cout << "add\n";
		std::cout << "Description: Add files and directories to have changes committed to next version.\n";
		std::cout << "Usage: svc add <loc1> <loc2> ...\n";
		std::cout << "Params:\n\tloc<n>: File/directory to track\n";
	}
}