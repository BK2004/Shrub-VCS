#ifndef ADD_H
#define ADD_H
#include "command.h"

namespace Commands {
	class Add : public Command {
	public:
		Add(ArgParse::ArgParser* parser) : Command(parser) {}
		void exec();
		void help();
	};
}

#endif