#ifndef KEYWORD_H
#define KEYWORD_H
#include "field.h"

namespace ArgParse {
	class Keyword : public Field {
	public:
		Keyword(std::string name, Field* parent, bool required) : Field(name, parent, required) {}
		bool match(std::vector<std::string>);
	private:
	};
}

#endif
