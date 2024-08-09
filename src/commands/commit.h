#ifndef COMMIT_H
#define COMMIT_H
#include "command.h"

namespace Commands {
	class Commit : public Command {
	public:
		Commit(ArgParse::ArgParser* parser) : Command(parser) {}
		void exec();
		void help();
	};
}

#endif