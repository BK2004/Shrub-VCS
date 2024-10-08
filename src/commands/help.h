#ifndef HELP_H
#define HELP_H
#include "command.h"

namespace Commands {
	class Help : public Command {
	public:
		Help(ArgParse::ArgParser* parser, std::map<std::string, Command*>* commands) : Command(parser), commands(commands) {}
		void exec();
		void help();
	private:
		std::map<std::string, Command*>* commands;
	};
}

#endif