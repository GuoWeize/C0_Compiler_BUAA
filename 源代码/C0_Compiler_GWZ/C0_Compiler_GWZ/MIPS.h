#ifndef MIPS_H
#define MIPS_H

#include "struct.h"
#include <iostream>
#include <string>
using namespace std;


class MIPS {
private:
	static string reg[32];			//寄存器名
	static int saved_reg[10];		//保存寄存器
	static int string_num;			//字符串标号
	static int alloc;				//全局地址
	static int offset;				//栈内偏移地址

public:
	MIPS();
	void initial ();	//初始化

	//常用函数
	void label (string label_name);		//放置函数标号
	void label (int num);				//放置标准标号
	int  alloc_addr (int size);			//申请全局地址
	int  alloc_stack (int size);		//申请栈内地址
	void new_offset ();					//新的offset
	void push ();			//寄存器入栈
	void pop ();			//恢复寄存器
	void scan_int ();		//读入整数
	void scan_char ();		//读入字符
	void print_int ();		//输出整数
	void print_char ();		//输出字符
	void print_string (string text);	//输出字符串

	//常用伪指令
	void li (int t1, int num);
	void la (int t1, string label, int t2);
	void move (int t1, int t2);

	//加减指令
	void add (int t1, int t2, int t3);
	void addi (int t1, int t2, int num);
	void sub (int t1, int t2, int t3);
	void subi (int t1, int t2, int num);

	//乘除指令
	void mfhi (int t1);
	void mflo (int t1);
	void mult (int t1, int t2);
	void div (int t1, int t2);

	//跳转指令
	void j (int label);
	void jr (int t1);
	void jal (string target);

	//分支指令
	void beq (int t1, int t2, int label);
	void bne (int t1, int t2, int label);
	void bge (int t1, int t2, int label);
	void bgt (int t1, int t2, int label);
	void ble (int t1, int t2, int label);
	void blt (int t1, int t2, int label);

	//存取指令(访存指令)
	void lw (int t1, int num, int t2);
	void sw (int t1, int num, int t2);
	
};

#endif
