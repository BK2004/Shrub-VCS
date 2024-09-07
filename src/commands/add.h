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
		std::filesystem::path dfs_add(const std::filesystem::directory_entry&, std::unordered_map<std::string, std::string>&);
		void help();
	private:
		bool wrote_new;
	};
}

#endif