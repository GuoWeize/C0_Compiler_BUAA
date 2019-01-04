#ifndef STRUCT_H
#define STRUCT_H
#include "symbol.h"

//词法单元
struct token {
	symbol sym;		//单词类型
	std::string id;	//名字
	int num;		//常量数值
};


//符号表项
struct record {
	std::string name;
	kind kind;		//类型
	int value;		//常量:值,   数组:元素个数,   函数:参数个数,    参数:序号,    变量:未定义
	int level;		//函数编号: 全局是0, 每进入一个新函数+1, 函数名是自身的编号
	int size;		//空间大小: 函数是返回值的空间大小; 数组是总大小
	int addr;		//内存地址: 常量变量是数据地址; 数组是起始地址; 函数是返回值的地址(数据段); 参数是栈内偏移地址
};


#endif