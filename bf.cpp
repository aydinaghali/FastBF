#include <iostream>
#include <string>
#include <cstdlib>

#include "Bfprog.h"

int main(int argc, char** argv){
	bool print_help = argc == 1;
	std::string subcommand;
	if(!print_help){
		subcommand = argv[1];
		print_help = subcommand == "help";
	}

	if(print_help){
		std::cout <<
			"Brainfuck interpreter and compiler.\n"
			"USAGE: bf {run|com|help} <file> [output]\n"
			"SUBCOMMANDS:\n"
			"\trun\t\tInterpret the file.\n"
			"\tcom\t\tCompile the file. The output filename can be provided, otherwise \"out\" will be used.\n"
			"\thelp\t\tPrint this help message.\n";
		exit(0);
	}

	Bfprog bf(256);
	Bfprog::Parse_Result res = bf.parse_file(argv[2]);
	if(res.status == Bfprog::Parse_Result::INVALID_FILE){
		std::cerr << "Invalid file: "<< argv[2] << std::endl;
		exit(1);
	}

	// TODO: fix error positioning
	if(res.status == Bfprog::Parse_Result::UNMATCHED_RBRACKET){
		std::cerr << "Unmatched right bracket at ("<<res.row<<", "<<res.column<<")"<< std::endl;
		exit(1);
	}else if(res.status == Bfprog::Parse_Result::UNMATCHED_LBRACKET){
		std::cerr << "Unmatched left bracket at ("<<res.row<<", "<<res.column<<")"<< std::endl;
		exit(1);
	}

	if(subcommand == "run")
		bf.run();
	else if(subcommand == "com"){
		bf.compile_to_c("/tmp/bf_com.c");
		std::string command("cc -o ");
		if(argc >= 4)
			command += argv[3];
		else
			command += "out";
		command += " /tmp/bf_com.c";
		system(command.c_str());
	}

	return 0;
}

