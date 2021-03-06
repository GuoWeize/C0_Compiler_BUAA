#include "symbol.h"
#include "Lexer.h"
#include "Parser.h"
#include <fstream>
using namespace std;

ifstream fin;	//输入文件名
ofstream fout;	//输出文件名
ofstream fcode;	//中间代码文件


int main () {
	cout << "Please input the input-file name:" << endl;
	string in_file;
	cin >> in_file;
	fin.open(in_file);

	string out_file = "out.txt";
	fout.open(out_file);		//清空上一次的结果
	fout.close();
	fout.open(out_file, ios::app);

	string code_file = "code.txt";
	fcode.open(code_file);		//清空上一次的结果
	fcode.close();
	fcode.open(code_file, ios::app);

	Error* error_p = new Error();
	Lexer* lexer_p = new Lexer (error_p);
	Table* table_p = new Table();
	MIPS*  mips_p  = new MIPS ();
	Code*  code_p =  new Code(table_p, mips_p);
	
	Parser parser = Parser(lexer_p, table_p, code_p);

	parser.program();

	return 0;
}
