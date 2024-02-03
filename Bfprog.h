#ifndef BFPROGRAM_H
#define BFPROGRAM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <stack>

class Bfprog {
public:
	Bfprog(unsigned int memory_size);
	~Bfprog();

	struct Token{
		enum{RIGHT=0,
			LEFT=1,
			PLUS=2,
			MINUS=3,
			OUT=4,
			IN=5,
			RBRACKET=6,
			LBRACKET=7} op;
		union{
			unsigned int count;
			//Token* match;
			std::list<Token>::iterator match;
		};
	};

	struct Parse_Result{
		enum {OK = 0,
			UNMATCHED_LBRACKET = 1,
			UNMATCHED_RBRACKET = 2,
			INVALID_FILE = 3 } status;
		unsigned int row, column;
	};

	struct Run_Result{
		enum {OK = 0,
			 } status;
	};

	struct Compile_Result{
		enum {OK = 0,
			INVALID_OUTFILE = 1
			 } status;
	};

	static constexpr const char* const op_names[8] = {"RIGHT", "LEFT",
		"PLUS", "MINUS", "OUT",
		"IN", "RBRACKET", "LBRACKET"};

	const std::list<Token>& get_token_list();
	const unsigned int get_memory_size();

	Parse_Result parse_string(const char* prog);
	Parse_Result parse_file(const char* filename);
	Run_Result run();
	Compile_Result compile_to_c(const char* outfilename);

private:
	std::vector<int> memory;
	unsigned int memory_size;
	unsigned int char_row, char_column;
	enum{COUNTING_NONE, COUNTING_PLUS, COUNTING_MINUS} counting;
	unsigned int op_count;
	std::list<Token> token_list;
	std::stack<std::list<Token>::iterator> rbracket_stack;
	struct{unsigned int row, column;} last_rbracket_position;

	Parse_Result parse_char(char c);

	#define OPFUNC_ARGS std::list<Token>::iterator& it, std::vector<int>& memory, unsigned int& header
	struct Opfuncs{
		static void right(OPFUNC_ARGS);
		static void left(OPFUNC_ARGS);
		static void plus(OPFUNC_ARGS);
		static void minus(OPFUNC_ARGS);
		static void out(OPFUNC_ARGS);
		static void in(OPFUNC_ARGS);
		static void rbracket(OPFUNC_ARGS);
		static void lbracket(OPFUNC_ARGS);
	};

	inline static void(*op_functions[8])(OPFUNC_ARGS) = {
		&Bfprog::Opfuncs::right, &Bfprog::Opfuncs::left, &Bfprog::Opfuncs::plus, &Bfprog::Opfuncs::minus, 
		&Bfprog::Opfuncs::out, &Bfprog::Opfuncs::in, &Bfprog::Opfuncs::rbracket, &Bfprog::Opfuncs::lbracket, 
	};

	#define OP_GENC_ARGS std::list<Token>::iterator& it, std::ofstream& outfile
	struct Op_genc{
		static void right(OP_GENC_ARGS);
		static void left(OP_GENC_ARGS);
		static void plus(OP_GENC_ARGS);
		static void minus(OP_GENC_ARGS);
		static void out(OP_GENC_ARGS);
		static void in(OP_GENC_ARGS);
		static void rbracket(OP_GENC_ARGS);
		static void lbracket(OP_GENC_ARGS);
	};

	inline static void(*op_genc_funcs[8])(OP_GENC_ARGS) = {
		&Bfprog::Op_genc::right, &Bfprog::Op_genc::left, &Bfprog::Op_genc::plus, &Bfprog::Op_genc::minus, 
		&Bfprog::Op_genc::out, &Bfprog::Op_genc::in, &Bfprog::Op_genc::rbracket, &Bfprog::Op_genc::lbracket, 
	};
};

std::ostream& operator<<(std::ostream& os, const Bfprog::Token& token);

#endif
