#include <iostream>
#include "argparse/argparse.h"

int main(int argc, char** argv) {
	ArgParse::ArgParser parser(argc, argv);
	ArgParse::Flag flag_list[] = { 
		ArgParse::Flag {"tails", 't'},
		ArgParse::Flag {"Tails", 'T'},
		ArgParse::Flag {"bottom", 'b'},
		ArgParse::Flag {"cat", '\0'},
	};
	try {
		auto flags = parser.match_flags(flag_list, 4);

		for (auto it = flags.begin(); it != flags.end(); it++) {
			std::cout << it->first << ": " << (it->second ? "true" : "false") << std::endl;
		}
	} catch (std::string e) {
		std::cout << "\033[1;31m" << e << "\033[0m" << std::endl;
	}
}