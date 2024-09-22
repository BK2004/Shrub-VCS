#ifndef ADD_H
#define ADD_H
#include <algorithm>
#include "command.h"
#include "objects.h"
#include "dict.h"
#include "repo.h"

namespace Commands {
	class Add : public Command {
	public:
		Add(ArgParse::ArgParser* parser) : Command(parser) {}
		void exec();
		void populate_add(DirTrie::DirTrie<bool>*);
		DirTrie::DirTrie<std::string>* update_staging(DirTrie::DirTrie<bool>*, DirTrie::DirTrie<std::string>*, std::filesystem::path&);
		std::filesystem::path dfs_hash(std::filesystem::path, std::unordered_map<std::string, std::string>&);
		void help();
	private:
		bool wrote_new;
	};
}

#endif