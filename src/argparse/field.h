#ifndef FIELD_H
#define FIELD_H
#include <string>

class Field {
public:
	virtual bool match(std::string*) = 0;
	std::string get_name() const { return name; };
private:
	std::string name;
};

#endif