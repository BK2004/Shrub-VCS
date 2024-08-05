#ifndef STRING_H
#define STRING_H
#include "field.h"

namespace ArgParse {
	class String : public Field {
	public:
		String(std::string name, Field* parent, bool required) : Field(name, parent, required) {}
		bool match(std::vector<std::string>);
	private:
	};
}

#endif
