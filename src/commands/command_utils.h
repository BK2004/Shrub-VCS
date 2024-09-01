#ifndef COMMAND_UTILS_H
#define COMMAND_UTILS_H
#include <filesystem>
#include <vector>
#include <fstream>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <cassert>

#define CHUNK 16384
#define SVC_DIR ".svc"

namespace Commands {
	std::filesystem::path get_svc_dir();
	std::filesystem::path create_obj(std::filesystem::path, std::string, std::vector<std::string>*);
}

#endif