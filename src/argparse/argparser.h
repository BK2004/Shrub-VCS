#ifndef ARGPARSER_H
#define ARGPARSER_H
#include "field.h"

namespace ArgParse {
	class ArgParser {
	public:
		ArgParser(Field*) { this->field = field; }
		~ArgParser() { delete this->field; }

		void parse(int, char**);
	private:
		Field* field;
	};
}

#endif