#ifndef COMMAND_UTILS_H
#define COMMAND_UTILS_H
#include <filesystem>
#include <vector>
#include <fstream>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <ctime>
#include "../libs/sha256.h"

#define CHUNK 16384
#define SVC_DIR ".svc"
#define LOG(logfile,contents) std::ofstream( get_svc_dir() / "logs" / logfile, std::ios::app ) << contents

namespace Commands {
	std::filesystem::path get_svc_dir();
	std::filesystem::path create_obj(std::filesystem::path, std::string, std::vector<std::string>*);
	std::filesystem::path create_commit_obj(std::string&, std::vector<std::string>&, std::string*);
	void update_ref(const char*, std::string);
	std::string get_ref(const char*);

	extern bool created_new; // flag = true if create_obj finds hashed file
}

#endif