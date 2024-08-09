#ifndef COMMAND_H
#define COMMAND_H
#include <filesystem>
#include <iostream>
#include "../argparse/argparse.h"

#define SVC_DIR ".svc"
#define SEE_HELP(cmd) std::cout << "For additional information, see svc help " #cmd "\n";

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