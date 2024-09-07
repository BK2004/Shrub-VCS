#ifndef DICT_H
#define DICT_H
#include <unordered_map>
#include <string>
#include <fstream>
#include "repo.h"

namespace Dict {
	std::unordered_map<std::string, std::string> read_dict_file(std::string);
	void write_dict_file(std::string, std::unordered_map<std::string, std::string>&);
}

#endif