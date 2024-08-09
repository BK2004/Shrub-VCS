#include "command_utils.h"


namespace Commands {
	// get_svc_dir
	// 	Get path of .svc directory in current directory
	// Params:
	// Returns:
	// 	Path to SVC_DIR if it exists, otherwise an empty path
	std::filesystem::path get_svc_dir() {
		std::filesystem::path dir = std::filesystem::current_path();
		while (dir.string() != dir.root_path() && dir.has_parent_path()) {
			if (std::filesystem::exists(dir.string() + "\\" + SVC_DIR)) {
				return std::filesystem::path(dir.string() + "\\" + SVC_DIR);
			} else {
				dir = dir.parent_path();
			}
		}

		dir.clear();
		return dir;
	}
}