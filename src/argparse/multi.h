#ifndef MULTI_H
#define MULTI_H
#include "field.h"

namespace ArgParse {
	class Multi : public Field {
	public:
		Multi(std::string name, Field* parent, bool required) : Field(name, parent, required) {}
		bool match(std::vector<std::string>);
	private:
	};
}

#endif
