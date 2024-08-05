#include "argparse.h"

// parse
// 	Parse command-line arguments
// 	Matches args with internal fields
// Params:
// 	arg_count - number of arguments
// 	args - list of args
// Returns:
// 	Matched fields
void ArgParse::parse(int arg_count, char** args) {
	if (args == nullptr) throw "Requires args";
	
	// convert args to std::string array

}