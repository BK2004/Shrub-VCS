# Generate commands.h header to include all commands in directory
rm -f argparse.h

write() {
	echo $1 >> argparse.h
}

FILES=`find . -name "**.h"`

write "#ifndef ARGPARSE_H"
write "#define ARGPARSE_H"
for file in $FILES
do
	write "#include \"$file\""
done
write "#endif"