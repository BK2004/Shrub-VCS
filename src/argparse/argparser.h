#ifndef ARGPARSER_H
#define ARGPARSER_H
#include <vector>
#include <string>
#include <unordered_map>
#include <ctype.h>
#include <format>

namespace ArgParse {
	struct Flag {
		std::string name;
		char short_name;
	};
	
	class ArgParser {
	public:
		ArgParser(int, char**);

		bool match_keyword(std::string);
		int match_int();
		std::unordered_map<std::string, bool> match_flags(Flag flags[], int);
		std::string match_arg();
		std::string match_word();
		std::string get_parsed(bool) const;
		bool is_parsed() const;
	private:
		int arg_count;
		int curr_arg;
		int flag_start;
		std::vector<std::string> args;
	};
}

#endif