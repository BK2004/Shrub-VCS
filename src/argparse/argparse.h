#ifndef ARGPARSE_H
#define ARGPARSE_H
#include "field.h"

class ArgParse {
public:
	ArgParse(Field*) { this->field = field; }

	void parse(int, char**);
private:
	Field* field;
};

#endif