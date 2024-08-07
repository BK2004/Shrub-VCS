#include "argparser.h"
#include <iostream>
namespace ArgParse {
	ArgParser::ArgParser(int arg_count, char** cli_args) {
		this->arg_count = arg_count - 1;
		this->curr_arg = 0;
		
		// Insert cli_args into args as std::strings and sort options to the back, keeping keywords in given order
		for (int i = 1; i < arg_count; i++) {
			this->args.push_back(std::string(cli_args[i]));
		}

		int l = 0;
		for (int i = 0; i < this->args.size(); i++) {
			if (this->args[i][0] != '-' || isdigit(this->args[i][1])) {
				// Not an option, move to start
				std::string tmp = this->args[i];
				this->args[i] = this->args[l];
				this->args[l++] = tmp;
			}
		}

		// l is start of flag args
		this->flag_start = l;
	}

	// ArgParser::match_keyword
	// 	Match keyword at top of args
	// 	If current arg matches, it is consumed and match returns true
	// 	Otherwise, match returns false
	// Params:
	// 	std::string keyword - keyword to match
	// Returns:
	// 	true if matched, false if not
	bool ArgParser::match_keyword(std::string keyword) {
		// If past flag start, can't match
		if (this->curr_arg >= this->flag_start) return false;

		if (keyword == this->args[curr_arg]) {
			curr_arg++;

			return true;
		} else {
			return false;
		}
	}

	// ArgParser::match_int
	// 	Matches int at top of args
	// 	If match is found, it is consumed and the value is returned
	// 	Otherwise, INT_MIN is returned
	// 	Note: doesn't account for overflow
	// Params:
	// Returns:
	// 	Value of integer, or INT_MIN if not a match
	int ArgParser::match_int() {
		// If past flag start, can't match
		if (this->curr_arg >= this->flag_start) return INT_MIN;

		int i = 0; int res = 0; bool negative = false;
		while (this->args[this->curr_arg][i] != '\0') {
			char c = this->args[this->curr_arg][i];
			if (c == '-' && i == 0) {
				negative = true;
			} else if (isdigit(c)) {
				res = res * 10 + (c - '0');
			} else {
				break;
			}
			i++;
		}

		if (i > 0 && this->args[this->curr_arg][i] == '\0') return res * (negative ? -1 : 1);

		return INT_MIN;
	}

	// ArgParser::match_flags
	// 	Matches provided flags with all flag args at end of file
	// 	If any of provided flag structs have conflicting names, an error will be thrown
	// Params:
	// 	flags - List of flags to match; flags with short_name = \0 are assumed to not have short form
	// Returns:
	// 	Map of flag names to true/false
	std::unordered_map<std::string, bool> ArgParser::match_flags(Flag flags[], int num_flags) {
		// Check for conflicts
		std::unordered_map<std::string, int> found_names;
		std::unordered_map<char, int> found_short_names;

		for (int i = 0; i < num_flags; i++) {
			if (found_names.count(flags[i].name) > 0) {
				throw "Flag " + flags[i].name + " can't be defined more than once.";
			} else {
				found_names[flags[i].name] = i;
			}

			if (flags[i].short_name != '\0') {
				if (found_short_names.count(flags[i].short_name) > 0) {
					throw "Flags " + flags[i].name + " and " + flags[found_short_names[flags[i].short_name]].name + " can't use the same short name (" + flags[i].short_name + ").";
				} else {
					found_short_names[flags[i].short_name] = i;
				}
			}
		}

		// No conflicts, populate flag map with names and link short names to long names
		std::unordered_map<std::string, bool> flag_map;
		std::unordered_map<char, std::string> short_to_name;
		for (int i = 0; i < num_flags; i++) {
			flag_map[flags[i].name] = false;
			if (flags[i].short_name != '\0') short_to_name[flags[i].short_name] = flags[i].name;
		}

		for (int i = this->flag_start; i < this->args.size(); i++) {
			if (this->args[i][1] == '-') {
				// Use full name (--)
				std::string name = this->args[i].substr(2);

				if (flag_map.count(name) > 0) {
					flag_map[name] = true;
				} else {
					// Not a valid flag
					throw "--" + name + " is not a recognized flag.";
				}
			} else {
				// Use short name (each char is a different option)
				std::string chars = this->args[i].substr(1);
				for (char c : chars) {
					if (short_to_name.count(c) > 0) {
						flag_map[short_to_name[c]] = true;
					} else {
						// Not a valid flag
						throw "-" + std::string(1, c) + " is not a recognized flag.";
					}
				}
			}
		}

		return flag_map;
	}

	// ArgParser::get_parsed
	// 	Get all parsed args
	// 	Include flags by setting incl_flags=true
	// Params:
	// Returns:
	// 	Space-delimited list of args that have been parsed (and flags if desired)
	std::string ArgParser::get_parsed(bool incl_flags) {
		std::string out = "";

		for (int i = 0; i < this->curr_arg; i++) {
			if (i == this->flag_start && !incl_flags) break;
			if (i > 0) {
				out += " ";
			}

			out += this->args[i];
		}

		return out;
	}
}