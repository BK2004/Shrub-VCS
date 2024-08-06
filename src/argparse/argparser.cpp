#include "argparser.h"
#include <iostream>

namespace ArgParse {
	// parse
	// 	Parse command-line arguments
	// 	Matches args with internal fields
	// Params:
	// 	arg_count - number of arguments
	// 	cli_args - list of args from command-line
	// Returns:
	// 	Matched fields
	ParsedArgs* ArgParser::parse(int arg_count, char** cli_args) {
		if (cli_args == nullptr) throw "Requires args";
		
		// Store args with options at back
		std::vector<std::string> args;

		for (int i = 1; i < arg_count; i++) {
			args.push_back(std::string(cli_args[i]));
		}

		// Sort non-option elements to l
		int l = 0;
		for (int i = 0; i < args.size(); i++) {
			if (args[i][0] != '-') {
				// Must be an option (starts with -), swap args[i] and args[l]
				std::string temp = args[i];
				args[i] = args[l];
				args[l++] = temp;
			}
		}

		return this->field->match(args, new ParsedArgs {});
	}
}