#ifndef OPTION_H
#define OPTION_H
#include "field.h"

namespace ArgParse {
	class Option : public Field {
	public:
		Option(std::string name, Field* parent, bool required) : Field(name, parent, required) {}
		ParsedArgs* match(std::vector<std::string>&, ParsedArgs*);
	private:
	};
}

#endif
