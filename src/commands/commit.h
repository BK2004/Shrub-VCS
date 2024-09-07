#ifndef COMMIT_H
#define COMMIT_H
#include <algorithm>
#include "command.h"
#include "objects.h"
#include "repo.h"

namespace Commands {
	class Commit : public Command {
	public:
		Commit(ArgParse::ArgParser* parser) : Command(parser), wrote_new(false) {}
		void exec();
		void help();
	private:
		bool wrote_new;
	};
}

#endif