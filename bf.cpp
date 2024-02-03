#include <iostream>
#include <string>
#include <cstdlib>

#include "Bfprog.h"

int main(int argc, char** argv){
	Bfprog bf(256);
	Bfprog::Parse_Result res = bf.parse_file(argv[1]);
	if(res.status == Bfprog::Parse_Result::INVALID_FILE){
		std::cerr << "Invalid file: "<< argv[1] << std::endl;
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

	std::string argv2(argv[2]);
	if(argv2 == "run")
		// TODO: fix input system for run
		bf.run();
	else if(argv2 == "com"){
		bf.compile_to_c("/tmp/bf_com.c");
		std::string command("cc -o ");
		command += argv[3];
		command += " /tmp/bf_com.c";
		system(command.c_str());
	}

	return 0;
}

