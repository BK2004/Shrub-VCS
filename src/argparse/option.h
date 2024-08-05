#ifndef OPTION_H
#define OPTION_H
#include "field.h"

namespace ArgParse {
	class Option : public Field {
	public:
		Option(std::string name, Field* parent, bool required) : Field(name, parent, required) {}
		bool match(std::vector<std::string>);
	private:
	};
}

#endif
