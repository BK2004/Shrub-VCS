#ifndef FIELD_H
#define FIELD_H
#include <string>
#include <vector>
#include <unordered_map>

namespace ArgParse {
	struct ParsedArgs {
		std::unordered_map<std::string, std::string> fields;
		std::unordered_map<std::string, bool> options;
	};

	class Field {
	public:
		Field(std::string name, Field* parent, bool required) : name(name), parent(parent), required(required) {
			this->flags = std::vector<Field*>();
		}
		virtual ParsedArgs* match(std::vector<std::string>&, ParsedArgs*) = 0;
		std::string get_name() const { return name; };
	private:
		std::string name;
		Field* parent;
		bool required;
		std::vector<Field*> flags;
	};
}

#endif