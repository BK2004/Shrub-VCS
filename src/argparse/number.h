#ifndef NUMBER_H
#define NUMBER_H
#include "field.h"

namespace ArgParse {
	class Number : public Field {
	public:
		Number(std::string name, Field* parent, bool required) : Field(name, parent, required) {}
		ParsedArgs* match(std::vector<std::string>&, ParsedArgs*);
	private:
	};
}

#endif
