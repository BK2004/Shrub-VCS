#ifndef DICT_H
#define DICT_H
#include <unordered_map>
#include <string>
#include <fstream>
#include "repo.h"
#include "dirtrie.h"

namespace Dict {
	std::unordered_map<std::string, std::string> read_dict_file(std::string);
	void write_dict_file(std::string, std::unordered_map<std::string, std::string>&);
	DirTrie::DirTrie<std::string>* build_trie(std::unordered_map<std::string, std::string>&);
	void write_trie_to_dict(DirTrie::DirTrie<std::string>*, std::string&);
}

#endif