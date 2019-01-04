#ifndef LEXER_H
#define LEXER_H

#include "struct.h"
#include "Error.h"
#include <iostream>
#include <string>
using namespace std;

//词法分析
class Lexer {
private:
	Error* error;	//错误处理对象
	char ch;		//处理的字符
	string id;		//读到的词法单元名(不转换大小写)
	token t;		//读到的词法单元
	int line;		//行计数
	int word;		//每行代码字符计数
	int is_over;	//判断是否结束
	void getch();	//读取一个字符

public:
	Lexer::Lexer(Error* err_p);
	token getsym();		//词法分析主函数:读取单词
	void all_exit();	//退出程序
	void has_err(string message);	//封装的报错函数(包含位置)
	
};

#endif