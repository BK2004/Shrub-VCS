#include <iostream>
#include "argparse/argparse.h"

int main(int argc, char** argv) {
	ArgParse::ArgParser(nullptr).parse(argc, argv);
}