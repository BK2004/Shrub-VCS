#include <iostream>
#include "argparse/argparse.h"

int main(int argc, char** argv) {
	(new ArgParse(nullptr))->parse(2, NULL);
}