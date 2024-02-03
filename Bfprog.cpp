#include "Bfprog.h"

Bfprog::Bfprog(unsigned int memory_size) : memory_size(memory_size) {
	memory = std::vector<int>(memory_size);
}

Bfprog::~Bfprog(){
}

std::ostream& operator<<(std::ostream& os, const Bfprog::Token& token){
	const char* op_name = Bfprog::op_names[token.op];
	os << '{' << op_name;
	if(token.op==Bfprog::Token::PLUS || token.op==Bfprog::Token::MINUS){
		os << ", Count: " << token.count;
	}else if(token.op==Bfprog::Token::RBRACKET || token.op==Bfprog::Token::LBRACKET){
		os << ", Match: ";
		if(token.match->op==Bfprog::Token::RBRACKET) os << "[";
		else if(token.match->op==Bfprog::Token::LBRACKET) os << "]";
		
		//os << ", Match addr:" << token.match;
		/*unsigned int i = 0;
		for (auto const& t : get_token_list()) {
			if(*token.match == t) break;
			++i;
		}
		os << ", Match index: " << i;*/
	}
	os << '}';
	return os;
}

const std::list<Bfprog::Token>& Bfprog::get_token_list(){
	return token_list;
}

const unsigned int Bfprog::get_memory_size(){
	return memory_size;
}

Bfprog::Parse_Result Bfprog::parse_string(const char* prog){
	counting = COUNTING_NONE;
	op_count = 0;
	char_row = 0;
	char_column = 0;

	unsigned i = 0;
	char c = prog[0];
	do{
		c = prog[i++];
		Parse_Result res = parse_char(c);
		if(res.status!=Parse_Result::OK) return res;
		if(c=='\n'){
			++char_row;
			char_column = 0;
		}else{
			++char_column;
		}
	}while(c != '\0');

	if(!rbracket_stack.empty())
		return {Parse_Result::UNMATCHED_RBRACKET,
			last_rbracket_position.row, last_rbracket_position.column};

	return {Parse_Result::OK, 0, 0};
}

Bfprog::Parse_Result Bfprog::parse_file(const char* filename){
	std::ifstream file(filename);
	if(!file.is_open()) return {Parse_Result::INVALID_FILE, 0};
	
	counting = COUNTING_NONE;
	op_count = 0;
	char_row = 0;
	char_column = 0;
	char c;
	do{
		c = file.get();
		Parse_Result res = parse_char(c);
		if(res.status!=Parse_Result::OK) return res;
		if(c=='\n'){
			++char_row;
			char_column = 0;
		}else{
			++char_column;
		}
	}while(c != EOF);
	parse_char('\0');

	if(!rbracket_stack.empty())
		return {Parse_Result::UNMATCHED_RBRACKET,
			last_rbracket_position.row, last_rbracket_position.column};

	return {Parse_Result::OK, 0, 0};
}

#define ADD_PLUS_OR_MINUS \
	if(counting==COUNTING_PLUS){ \
		token_list.push_back(Token{Token::PLUS, op_count}); \
		op_count = 0; \
		counting = COUNTING_NONE; \
	}else if(counting==COUNTING_MINUS){ \
		token_list.push_back(Token{Token::MINUS, op_count}); \
		op_count = 0; \
		counting = COUNTING_NONE; \
	} \

Bfprog::Parse_Result Bfprog::parse_char(char c){
	// TODO: replace push_back() with emplace_back()
	switch(c){
		case '>':
			ADD_PLUS_OR_MINUS
			token_list.push_back(Token{Token::RIGHT, 0});
			break;
		case '<':
			ADD_PLUS_OR_MINUS
			token_list.push_back(Token{Token::LEFT, 0});
			break;
		case '+':
			if(counting==COUNTING_MINUS){
				token_list.push_back(Token{Token::MINUS, op_count});
				op_count = 0;
			}
			counting = COUNTING_PLUS;
			++op_count;
			break;
		case '-':
			if(counting==COUNTING_PLUS){
				token_list.push_back(Token{Token::PLUS, op_count});
				op_count = 0;
			}
			counting = COUNTING_MINUS;
			++op_count;
			break;
		case '.':
			ADD_PLUS_OR_MINUS
			token_list.push_back(Token{Token::OUT, 0});
			break;
		case ',':
			ADD_PLUS_OR_MINUS
			token_list.push_back(Token{Token::IN, 0});
			break;
		case '[':
			ADD_PLUS_OR_MINUS
			token_list.push_back(Token{Token::RBRACKET, 0});
			rbracket_stack.push(std::prev(token_list.end()));
			last_rbracket_position = {char_row, char_column};
			break;
		case ']':
			ADD_PLUS_OR_MINUS
			token_list.push_back(Token{Token::LBRACKET, 0});
			if(rbracket_stack.empty())
				return {Parse_Result::UNMATCHED_LBRACKET, char_row, char_column};
			rbracket_stack.top()->match = std::prev(token_list.end());
			token_list.back().match = rbracket_stack.top();
			rbracket_stack.pop();
			break;
		case '\0':
			ADD_PLUS_OR_MINUS
			break;
		default:
			break;
	}

	return {Parse_Result::OK, 0, 0};
}

//#define DEBUG_OPFUNC
#ifdef DEBUG_OPFUNC
	#define DEBUG_OPFUNC_PRINT(x) std::cout<<(x)<<std::endl
#else
	#define DEBUG_OPFUNC_PRINT(x)
#endif
/*void Bfprog::Opfuncs::right(OPFUNC_ARGS){
	std::cout << i << std::endl;
	DEBUG_OPFUNC_PRINT("Right called");
}*/

void Bfprog::Opfuncs::right(OPFUNC_ARGS){
	++header;
	DEBUG_OPFUNC_PRINT("Right called");
}

void Bfprog::Opfuncs::left(OPFUNC_ARGS){
	--header;
	DEBUG_OPFUNC_PRINT("Left called");
}

void Bfprog::Opfuncs::plus(OPFUNC_ARGS){
	memory[header] += it->count;
	DEBUG_OPFUNC_PRINT("Plus called");
}
void Bfprog::Opfuncs::minus(OPFUNC_ARGS){
	memory[header] -= it->count;
	DEBUG_OPFUNC_PRINT("Minus called");
}
void Bfprog::Opfuncs::out(OPFUNC_ARGS){
	std::cout<<(char)memory[header];
	DEBUG_OPFUNC_PRINT("Out called");
}
void Bfprog::Opfuncs::in(OPFUNC_ARGS){
	std::string str;
	std::cout<<"> ";
	std::cin>>str;
	memory[header] = (int)str[0];
	DEBUG_OPFUNC_PRINT("In called");
}
void Bfprog::Opfuncs::rbracket(OPFUNC_ARGS){
	if(memory[header] == 0)
		it = it->match;
	DEBUG_OPFUNC_PRINT("Rbracket called");
}
void Bfprog::Opfuncs::lbracket(OPFUNC_ARGS){
	if(memory[header] != 0)
		it = it->match;
	DEBUG_OPFUNC_PRINT("Lbracket called");
}

Bfprog::Run_Result Bfprog::run(){
	unsigned int header = memory_size/2;
	//unsigned int header = 0;

	std::list<Token>::iterator it = token_list.begin();
	while(it != token_list.end()){
		op_functions[it->op](it, memory, header);
#ifdef DEBUG_OPFUNC
		std::cout<<"> "<<header<<" ["<<memory[0]<<','<<memory[1]<<']'<<std::endl;
#endif
		++it;
	}

	return {Run_Result::OK};
}

void Bfprog::Op_genc::right(OP_GENC_ARGS){
	outfile << "++p;" << std::endl;
}

void Bfprog::Op_genc::left(OP_GENC_ARGS){
	outfile << "--p;" << std::endl;
}

void Bfprog::Op_genc::plus(OP_GENC_ARGS){
	outfile << "*p += " << it->count << ";" << std::endl;
}

void Bfprog::Op_genc::minus(OP_GENC_ARGS){
	outfile << "*p -= " << it->count << ";" << std::endl;
}

void Bfprog::Op_genc::out(OP_GENC_ARGS){
	outfile << "putchar((char)(*p));" << std::endl;
}

void Bfprog::Op_genc::in(OP_GENC_ARGS){
	outfile << "*p = (int)getchar();" << std::endl;
}

void Bfprog::Op_genc::rbracket(OP_GENC_ARGS){
	outfile << "while(*p){" << std::endl;
}

void Bfprog::Op_genc::lbracket(OP_GENC_ARGS){
	outfile << "}" << std::endl;
}

Bfprog::Compile_Result Bfprog::compile_to_c(const char* outfilename){
	std::ofstream outfile(outfilename);
	if(!outfile.is_open()) return {Compile_Result::INVALID_OUTFILE};

	outfile << "#include <stdio.h>" << std::endl;
	outfile << "#include <stdlib.h>" << std::endl;
	outfile << "int main(){" << std::endl;
	outfile << "int* m = (int*)malloc(" << memory_size << "*sizeof(int));" << std::endl;
	outfile << "int* p = m + " << memory_size/2 << ";" << std::endl;

	std::list<Token>::iterator it = token_list.begin();
	while(it != token_list.end()){
		op_genc_funcs[it->op](it, outfile);
		++it;
	}

	outfile << "return 0;" << std::endl;
	outfile << "}" << std::endl;

	return {Compile_Result::OK};
}
