#include <iostream>
#include <unordered_map>
#include "./commands/commands.h"
#include "dict.h"

#define ADD_CMD(name, class) cmds[#name] = new Commands:: class (&parser);

void print_valid(char**, std::map<std::string, Commands::Command*>);

int main(int argc, char** argv) {
	ArgParse::ArgParser parser(argc, argv);
	
	std::map<std::string, Commands::Command*> cmds;
	cmds["help"] = new Commands::Help(&parser, &cmds); 
	ADD_CMD(init, Init)
	ADD_CMD(commit, Commit)
	ADD_CMD(add, Add)

	// Look for command
	Commands::Command* cmd = nullptr;
	std::string cmd_name = parser.match_word();
	if (!cmd_name.empty() && cmds.count(cmd_name) > 0) {
		cmd = cmds[cmd_name];
	} 

	if (cmd) cmd->exec();
	else print_valid(argv, cmds);
}

void print_valid(char** argv, std::map<std::string, Commands::Command*> cmds) {
	std::cout << "Valid commands:";

	// Output all commands in cmds
	for (auto it = cmds.begin(); it != cmds.end(); it++) {
		if (it != cmds.begin()) {
			std::cout << ",";
		}
		std::cout << " " << it->first;
	}
	std::cout << std::endl;

	std::cout << "For more information, see svc help";
}