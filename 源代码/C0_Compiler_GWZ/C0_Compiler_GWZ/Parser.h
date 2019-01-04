#ifndef PARSER_H
#define PARSER_H

#include "struct.h"
#include "symbol.h"
#include "Lexer.h"
#include "Table.h"
#include "Code.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Parser {
private:
	Lexer* lexer;	//词法分析对象
	Table* table;	//符号表
	Code*  code;	//中间代码生成器

	token t;		//处理的单词
	record r;		//处理的符号表项
	int is_forward;	//预读个数
	token forward1, forward2;	//预读的单词
	vector<symbol> all_sym;		//语句块和声明结束
	void read();			//读入一个单词
	token forwardread();	//预读一个单词
	void skip();			//跳读
	void skip(vector<symbol> fsys);	//跳读
	void newrecord();		//重置记录r
	bool is_same_kind (kind const_kind, kind other);		//判断常量类型是否一致
	bool is_return (kind function_kind, kind return_kind);	//判断返回类型是否一致
	bool is_para (kind para_kind, kind expr_kind);			//判断参数类型是否一致

public:
	Parser(Lexer* lexer, Table* table_p, Code* code_p);

	//小的语法项目
	int integer ();			//整数(有符号和0)
	record constant ();		//字面常量
	void arraydec ();		//数组定义 ‘[’ Integer ‘]’
	record arraycall (record array_name);	//数组引用 ‘[’ <表达式> ‘]’
	int condition ();		//条件
	record factor ();		//因子
	record term ();			//项
	record expression ();	//表达式
	int formalpara ();		//形参表
	void actualpara (record function);	//实参表
	
	//语句项目
	void constdec ();		//常量声明
	void constdef ();		//常量定义
	void vardec ();			//变量声明
	void vardef ();			//变量定义
	void funcdef ();		//函数定义
	void statement ();		//单个语句
	void stats ();			//复合语句
	record funcall ();		//函数调用
	void assignstat ();		//赋值语句
	void ifstat ();			//if语句
	void whilestat ();		//while语句
	void casestat (record switch_on, int end_label);	//case语句
	void defaultstat ();	//default语句
	void switchstat ();		//switch语句
	void returnstat ();		//return语句
	void scanfstat ();		//输入Input
	void printfstat ();		//输出Output
	
	//程序部分
	void compoundstats ();	//复合语句
	void program ();		//语法分析入口,开始符号
	
};

#endif
