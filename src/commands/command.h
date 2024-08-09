#ifndef COMMAND_H
#define COMMAND_H
#include <iostream>
#include "../argparse/argparse.h"
#include "command_utils.h"

#define SEE_HELP(cmd) std::cout << "For additional information, see svc help " #cmd "\n";
#define TEXT_RED "\033[1;31m"
#define TEXT_RESET "\033[0m"
#define ERR(text) std::cout << TEXT_RED << text << TEXT_RESET << std::endl;

namespace Commands {
	class Command {
	public:
		Command(ArgParse::ArgParser* parser): parser(parser) {}
		~Command() {}

		virtual void exec() = 0;
		virtual void help() = 0;
	protected:
		ArgParse::ArgParser* parser;
	};
}

#endif