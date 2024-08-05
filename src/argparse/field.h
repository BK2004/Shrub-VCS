#ifndef FIELD_H
#define FIELD_H
#include <string>
#include <vector>

namespace ArgParse {
	class Field {
	public:
		Field(std::string name, Field* parent, bool required) : name(name), parent(parent), required(required) {
			this->flags = std::vector<Field*>();
		}
		virtual bool match(std::vector<std::string>) = 0;
		std::string get_name() const { return name; };
	private:
		std::string name;
		Field* parent;
		bool required;
		std::vector<Field*> flags;
	};
}

#endif