#ifndef MULTI_H
#define MULTI_H
#include "field.h"

namespace ArgParse {
	class Multi : public Field {
	public:
		Multi(std::string name, Field* parent, bool required) : Field(name, parent, required) {}
		ParsedArgs* match(std::vector<std::string>&, ParsedArgs*);
	private:
	};
}

#endif
