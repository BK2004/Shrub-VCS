#include "init.h"

namespace Commands {
	void Init::exec() {
		const std::string subdirs[] = {"objects", "refs", "logs/commits"};

		std::string arg = this->parser->match_arg();
		if (arg.empty() || !this->parser->is_parsed()) {
			ERR("Usage: " << this->parser->get_parsed(false) << " <dir>")
			SEE_HELP(init)
			return;
		}

		std::filesystem::path path = arg + "/" + SVC_DIR;

		if (!Repo::get_svc_dir().empty()) {
			std::cout << arg << " is already being tracked." << std::endl;
			return;
		}

		std::filesystem::create_directories(path);
		for (std::string subdir : subdirs) {
			std::filesystem::create_directories(path.string() + "/" + subdir);
		}
		std::cout << "Tracking " << arg << std::endl;
	}

	void Init::help() {
		std::cout << "init\n";
		std::cout << "Description: Initialize ShrubVCS tracking on a directory\n";
		std::cout << "Usage: svc init <dir>\n";
		std::cout << "Params:\n";
		std::cout << "\tdir: Directory to track\n";
	}
}