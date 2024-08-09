#ifndef COMMAND_UTILS_H
#define COMMAND_UTILS_H
#include <filesystem>

#define SVC_DIR ".svc"

namespace Commands {
	std::filesystem::path get_svc_dir();
}

#endif