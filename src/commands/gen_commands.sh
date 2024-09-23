#!/usr/bin/env bash
# Generate commands.h header to include all commands in directory
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR
rm -f commands.h

write() {
	echo $1 >> commands.h
}

FILES=`find . -name "**.h"`

write "#ifndef COMMANDS_H"
write "#define COMMANDS_H"
for file in $FILES
do
	write "#include \"$file\""
done
write "#endif"