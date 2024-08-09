#ifndef INIT_H
#define INIT_H
#include "command.h"

namespace Commands {
	class Init : public Command {
	public:
		Init(ArgParse::ArgParser* parser) : Command(parser) {}
		void exec();
		void help();
	};
}

#endif