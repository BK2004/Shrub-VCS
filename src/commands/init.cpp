#include "init.h"

namespace Commands {
	void Init::exec() {
		std::string arg = this->parser->match_arg();
		if (arg.empty() || !this->parser->is_parsed()) {
			std::cout << "Usage: " << this->parser->get_parsed(false) << " <dir>" << std::endl;
		}

		std::filesystem::path path = arg + "\\" + VCS_DIR;

		if (std::filesystem::is_directory(path)) {
			std::cout << arg << " is already being tracked." << std::endl;
			return;
		}

		std::filesystem::create_directories(path);
		std::cout << "Tracking " << arg << std::endl;
	}
}