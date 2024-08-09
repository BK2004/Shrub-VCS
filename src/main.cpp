#include <iostream>
#include "./commands/commands.h"

int main(int argc, char** argv) {
	ArgParse::ArgParser parser(argc, argv);
	
	Commands::Command* cmd = nullptr;
	if (parser.match_keyword("init")) {
		cmd = new Commands::Init(&parser);
	}

	if (cmd) cmd->exec();
	else std::cout << "Valid commands: init" << std::endl;
}