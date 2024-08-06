#ifndef ARGPARSER_H
#define ARGPARSER_H
#include "field.h"

namespace ArgParse {
	struct ParsedArgs {
		std::unordered_map<std::string, std::string> fields;
		std::unordered_map<std::string, bool> options;
	};

	class ArgParser {
	public:
		ArgParser(Field*) { this->field = field; }
		~ArgParser() { delete this->field; }

		ParsedArgs* parse(int, char**);
	private:
		Field* field;
	};
}

#endif