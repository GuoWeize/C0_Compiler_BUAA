#ifndef CODE_H
#define CODE_H

#include <iostream>
#include <string>
#include "struct.h"
#include "Table.h"
#include "MIPS.h"
using namespace std;

class Code {
private:
	int lnum;		//Label标号
	int tnum;		//临时变量标号
	Table* table;	//符号表
	MIPS* mips;		//MIPS生成器

public:
	Code(Table* table_p, MIPS* mips_p);
	//常用函数
	int  getlabel ();					//返回新Label
	int  putlabel ();					//放置新Label,并返回
	void putlabel (int label);			//放置已有的Label
	record new_temp (kind temp_kind);	//返回新的临时变量
	
	//特殊过程
	void alloc_add (record r);				//添加声明
	void funcdec (record function);			//函数声明
	void para (record parameter);			//参数声明
	void force ();							//强制返回
	void ret ();							//返回语句
	void ret (record func, record name);	//返回语句,有返回值
	void scan (record name);				//输入变量 scanf name
	void print (record name);				//输出变量 printf name
	void prints (string text);				//输出字符串 printf text
	record call (record func);				//调用语句 call func
	void para_trans (record para, record expr);		//参数传递 push para
	void assign (record left, record right);		//赋值语句 left=right
	void load (int reg, record rec);		//取值
	void store (int reg, record rec);		//存入

	//计算过程
	record neg (record num);					//取反
	record plus (record op1, record op2);		//加法
	record minus (record op1, record op2);		//减法
	record mult (record op1, record op2);		//乘法
	record div (record op1, record op2);		//除法
	record array_call (record array_name, record index);	//数组引用 ^1 === array_name[index]

	//跳转过程
	int  condition (record single);					//单一表达式条件, 不满足跳转到新Label
	int  equal (record left, record right);			//条件 left == right, 不满足跳转到新Label
	int  notequal (record left, record right);		//条件 left != right, 不满足跳转到新Label
	int  big (record left, record right);			//条件 left > right, 不满足跳转到新Label
	int  bigequal (record left, record right);		//条件 left >= right, 不满足跳转到新Label
	int  less (record left, record right);			//条件 left < right, 不满足跳转到新Label
	int  lessequal (record left, record right);		//条件 left <= right, 不满足跳转到新Label

	int  jump ();				//无条件跳转到新Label
	void jump (int label);		//无条件跳转到已有的Label

};

#endif 
