#ifndef COMMAND_H
#define COMMAND_H
#include <filesystem>
#include <iostream>
#include "../argparse/argparse.h"

#define VCS_DIR ".vcs"

namespace Commands {
	class Command {
	public:
		Command(ArgParse::ArgParser* parser): parser(parser) {}
		~Command() {}

		virtual void exec() = 0;
	protected:
		ArgParse::ArgParser* parser;
	};
}

#endif