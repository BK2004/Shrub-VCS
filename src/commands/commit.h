#ifndef COMMIT_H
#define COMMIT_H
#include <algorithm>
#include "command.h"

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