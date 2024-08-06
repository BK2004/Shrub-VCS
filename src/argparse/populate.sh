FILES="keyword multi number option string"
TRG=""

write () {
	echo -e "$1" >> $TRG
}

# Write preamble to argparse.h
rm -f argparse.h
TRG=argparse.h
write "#ifndef ARGPARSE_H"
write "#define ARGPARSE_H"
write "#include \"argparser.h\""
write "#include \"field.h\""

for file in $FILES
do
	rm -f $file.cpp $file.h

	# Write header file
	TRG=$file.h
	write "#ifndef ${file^^}_H"
	write "#define ${file^^}_H"
	write "#include \"field.h\""
	write "\nnamespace ArgParse {"
	write "	class ${file^} : public Field {"
	write "	public:"
	write "		${file^}(std::string name, Field* parent, bool required) : Field(name, parent, required) {}"
	write "		ParsedArgs* match(std::vector<std::string>&, ParsedArgs*);"
	write "	private:"
	write "	};\n}\n"
	write "#endif"

	# Write cpp file
	TRG=$file.cpp
	write "#include \"$file.h\""
	write "\nnamespace ArgParse {"
	write "	ParsedArgs* ${file^}::match(std::vector<std::string>& args, ParsedArgs* parsed_args) {"
	write "		return parsed_args;"
	write "	}\n}"

	# Write include for argparse.h
	TRG=argparse.h
	write "#include \"$file.h\""
done

# postamble for argparse.h
TRG=argparse.h
write "#endif"