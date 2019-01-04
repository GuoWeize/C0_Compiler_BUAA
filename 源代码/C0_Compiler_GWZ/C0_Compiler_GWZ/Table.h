#ifndef TABLE_H
#define TABLE_H

#include "struct.h"
#include "symbol.h"
#include "Lexer.h"
#include <iostream>
#include <string>
#include <vector>

class Table {
private:
	static int num;			//函数编号
	vector<record> global;	//全局变量记录
	vector<record> t;		//符号表记录
	vector<record>::iterator iter;	//迭代器

public:
	Table();
	int level;		//运行时当前函数编号, 0是全局
	bool is_found (string name);			//查全局或数据表, 标识符是否重复定义
	bool is_found_global (string name);		//查全局表, 标识符是否重复定义
	record find (string name);			//查全局和数据表, 返回记录  未找到返回addr为-1的记录
	record find_global (string name);	//查全局表, 返回记录  未找到返回addr为-1的记录
	record last_global ();				//全局表中最后一个函数记录
	record find_para (int func_level, int para_index);	//查找参数
	void add (record r);				//在数据表添加
	void add_global(record r);			//在全局表添加
	void write_value (int value);		//回填value
	void write_addr (int addr);			//回填addr
	void new_level ();					//函数编号num+1, 并赋值给level

};

#endif 
