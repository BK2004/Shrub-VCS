#ifndef OBJECTS_H
#define OBJECTS_H
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <openssl/sha.h>
#include <zlib.h>
#include "repo.h"
#include "sha256.h"

namespace Objects {
	std::filesystem::path create_obj(std::filesystem::path, std::string, std::vector<std::string>*);
	std::filesystem::path create_commit_obj(std::string&, std::vector<std::string>&, std::string*);
}

#endif