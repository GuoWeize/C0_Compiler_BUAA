#include "MIPS.h"
#include "Error.h"
#include <fstream>

using namespace std;
extern ofstream fout;

/*寄存器名*/
string MIPS::reg[32] = {
	"$0 ", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
	"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
	"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
	"$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

/*保存寄存器*/
int MIPS::saved_reg[10] = {
	16, 17, 18, 19, 20, 21, 22, 23, 30, 31
};

int MIPS::offset = -40;			//栈内偏移地址位置
int MIPS::alloc = 268697600;	//全局空闲地址位置
int MIPS::string_num = 1;		//字符串标号

MIPS::MIPS () {
	initial();
}


/*初始化*/
void MIPS::initial() {
	fout << "#Compiler's result#" << endl;
	fout << "###################" << endl;
	fout << "move $fp,$sp" << endl;

	fout << ".data" << endl;
	fout << "$string0: .asciiz \"\\n\"" << endl;
	fout << ".text" << endl;

	fout << "jal main" << endl;
	fout << "li $v0,10" << endl;
	fout << "syscall" << endl;
	fout << endl;
}


/*常用函数*/
////////////////////////////////////////////////////////////////////////////////////////////////

/*放置函数标号*/
void MIPS::label(string label_name) {
	fout << endl << label_name << ":" << endl;
}

/*放置标准标号*/
void MIPS::label(int num) {
	fout << "$L" << num << ":" << endl;
}

/*申请全局地址*/
int MIPS::alloc_addr(int size) {
	alloc += size;
	return alloc - size;
}

/*申请栈内地址*/
int MIPS::alloc_stack(int size) {
	offset -= size;
	addi(29, 30, offset);
	return offset;
}

/*新的offset*/
void MIPS::new_offset() {
	offset = -40;
}

/*寄存器入栈*/
void MIPS::push() {
	for (int i = 0; i < 10; i++) {
		sw(saved_reg[i], i*(-4) - 4, 29);
	}
}

/*恢复寄存器*/
void MIPS::pop() {
	for (int i = 0; i < 10; i++) {
		lw(saved_reg[i], i*(-4) - 4, 29);
	}
}

/*读入整数*/
void MIPS::scan_int() {
	fout << "li $v0,5" << endl;
	fout << "syscall" << endl;
}

/*读入字符*/
void MIPS::scan_char() {
	fout << "li $v0,12" << endl;
	fout << "syscall" << endl;
}

/*输出整数*/
void MIPS::print_int() {
	fout << "li $v0,1" << endl;
	fout << "syscall" << endl;
	
	fout << "li $v0,4" << endl;
	fout << "la $a0,$string0" << endl;
	fout << "syscall" << endl;
}

/*输出字符*/
void MIPS::print_char() {
	fout << "li $v0,11" << endl;
	fout << "syscall" << endl;
	
	fout << "li $v0,4" << endl;
	fout << "la $a0,$string0" << endl;
	fout << "syscall" << endl;
}

/*输出字符串*/
void MIPS::print_string(string text) {
	fout << ".data" << endl;
	fout << "$string" << string_num << ": .asciiz " << "\"" << text << "\\n" << "\"" << endl;
	fout << ".text" << endl;
	fout << "li $v0,4" << endl;
	fout << "la $a0,$string" << string_num << endl;
	fout << "syscall" << endl;

	string_num++;
}



/*具体指令*/
////////////////////////////////////////////////////////////////////////////////////////////////

/*$t1 = num*/
void MIPS::li(int t1, int num) {
	fout << "li " << reg[t1] << "," << num << endl;
}

/*$t1={label}+$t2*/
void MIPS::la(int t1, string label, int t2) {
	fout << "la " << reg[t1] << "," << label << "(" << reg[t2] << ")" << endl;
}

/*$t1=$t2*/
void MIPS::move(int t1, int t2) {
	fout << "move " << reg[t1] << "," << reg[t2] << endl;
}

/*$t1=$t2+$t3, signed, with overflow*/
void MIPS::add(int t1, int t2, int t3) {
	fout << "add " << reg[t1] << "," + reg[t2] << "," + reg[t3] << endl;
}

/*$t1=$t2+num, signed*/
void MIPS::addi(int t1, int t2, int num) {
	fout << "addi " << reg[t1] << "," << reg[t2] << "," << num << endl;
}

/*$t1=$t2-$t3, signed, with overflow*/
void MIPS::sub(int t1, int t2, int t3) {
	fout << "sub " << reg[t1] << "," + reg[t2] << "," + reg[t3] << endl;
}

/*$t1=$t2-num, signed*/
void MIPS::subi(int t1, int t2, int num) {
	fout << "subi " << reg[t1] << "," << reg[t2] << "," << num << endl;
}

/*$t1=HI*/
void MIPS::mfhi(int t1) {
	fout << "mfhi " << reg[t1] << endl;
}

/*$t1=LO*/
void MIPS::mflo(int t1) {
	fout << "mflo " << reg[t1] << endl;
}

/*(HI,LO)=$t1*$t2, signed*/
void MIPS::mult(int t1, int t2) {
	fout << "mult " << reg[t1] << "," << reg[t2] << endl;
}

/*LO=$t1/$t2, HI=$t1%$t2, signed, with overflow*/
void MIPS::div(int t1, int t2) {
	fout << "div " << reg[t1] << "," << reg[t2] << endl;
}

/*PC=Label*/
void MIPS::j(int label) {
	fout << "j $L" << label << endl;
}

/*PC=$t1*/
void MIPS::jr(int t1) {
	fout << "jr " << reg[t1] << endl;
}

/*$ra=PC, PC=target*/
void MIPS::jal(string target) {
	fout << "jal " << target << endl;
}

/*if($t1=$t2/num) PC={label}*/
void MIPS::beq(int t1, int t2, int label) {
	fout << "beq " << reg[t1] << "," << reg[t2] << "," << "$L" << label << endl;
}

/*if($t1!=$t2/num) PC={label}*/
void MIPS::bne(int t1, int t2, int label) {
	fout << "bne " << reg[t1] << "," << reg[t2] << "," << "$L" << label << endl;
}

/*if($t1>=$t2/num) PC={label}*/
void MIPS::bge(int t1, int t2, int label) {
	fout << "bge " << reg[t1] << "," << reg[t2] << "," << "$L" << label << endl;
}

/*if($t1>$t2/num) PC={label}*/
void MIPS::bgt(int t1, int t2, int label) {
	fout << "bgt " << reg[t1] << "," << reg[t2] << "," << "$L" << label << endl;
}

/*if($t1<=$t2/num) PC={label}*/
void MIPS::ble(int t1, int t2, int label) {
	fout << "ble " << reg[t1] << "," << reg[t2] << "," << "$L" << label << endl;
}

/*if($t1<$t2/num) PC={label}*/
void MIPS::blt(int t1, int t2, int label) {
	fout << "blt " << reg[t1] << "," << reg[t2] << "," << "$L" << label << endl;
}

/*$t1=m[$t2+num]*/
void MIPS::lw(int t1, int num, int t2) {
	fout << "lw " << reg[t1] << "," << num << "(" << reg[t2] << ")" << endl;
}

/*m[$t2+num]=$t1*/
void MIPS::sw(int t1, int num, int t2) {
	fout << "sw " << reg[t1] << "," << num << "(" << reg[t2] << ")" << endl;
}
