#ifndef COMMIT_H
#define COMMIT_H
#include "command.h"

namespace Commands {
	class Commit : public Command {
	public:
		Commit(ArgParse::ArgParser* parser) : Command(parser), wrote_new(false) {}
		void exec();
		std::filesystem::path dfs_commit(const std::filesystem::directory_entry&);
		void help();
	private:
		bool wrote_new;
	};
}

#endif