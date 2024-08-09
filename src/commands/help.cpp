#include "help.h"

namespace Commands {
	void Help::exec() {
		std::string cmd = this->parser->match_word();
		if (cmd.empty() && this->parser->is_parsed()) {
			// List all commands
			std::cout << "List of commands:\n";
			for (auto it = this->commands->begin(); it != this->commands->end(); it++) {
				std::cout << "\t" << it->first << std::endl;
			}
			std::cout << "For more info on any of these commands, see vcs help <cmd>\n";
		} else if (!cmd.empty() && this->parser->is_parsed()) {
			// If command exists, run its help function, otherwise notify that it isn't a registered command
			if (this->commands->count(cmd) > 0) {
				(*this->commands)[cmd]->help();
			} else {
				ERR(cmd << " is not a valid command")
				std::cout << "For a list of valid commands, see vcs help\n";
			}
		} else {
			// Invalid usage
			ERR("Usage: svc help <cmd>\n\tsvc help");
		}
	}

	void Help::help() {
		std::cout << "help\n";
		std::cout << "Description: Get help with a command or list commands that can be used\n";
		std::cout << "Usage: svc help <cmd>\n";
		std::cout << "\tsvc help\n";
		std::cout << "Params:\n";
		std::cout << "\tcmd: Name of command to get help with (optional)\n";
	}
}