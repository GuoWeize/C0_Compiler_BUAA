#include "Code.h"
#include "Error.h"
#include <fstream>
#include <algorithm>

using namespace std;
extern ofstream fcode;


Code::Code(Table* table_p, MIPS * mips_p) {
	table = table_p;
	mips = mips_p;
	lnum = 0;
	tnum = 0;
}


/*���ú���*/
////////////////////////////////////////////////////////////////////////////////////////////////

/*������Label*/
int Code::getlabel() {
	lnum++;
	return lnum;
}

/*������Label,������*/
int Code::putlabel() {
	lnum++;
	mips->label(lnum);
	fcode << "L" << lnum << ": " << endl;
	return lnum;
}

/*�������е�Label*/
void Code::putlabel(int label) {
	mips->label(label);
	fcode << "L" << label << ": " << endl;
}

/*�����µ���ʱ����*/
record Code::new_temp(kind temp_kind) {
	tnum++;
	record r;
	r.size = 4;
	r.kind = temp_kind;
	r.level = table->level;
	r.addr = mips->alloc_stack(4);
	r.name = "^" + to_string(static_cast<long long>(tnum));
	return r;
}


/*�������*/
////////////////////////////////////////////////////////////////////////////////////////////////


/*�������*/
void Code::alloc_add(record r) {
	if (table->level == 0) {
		r.addr = mips->alloc_addr(r.size);
		table->add_global(r);
	}
	else {
		r.addr = mips->alloc_stack(r.size);
		table->add(r);
	}
}


/*�������� int function()*/
void Code::funcdec(record function) {
	mips->label(function.name);
	mips->move(30, 29);
	mips->new_offset();
	mips->addi(29, 29, -40);

	if (function.kind != voidfunc && function.kind != npvoidfunc) {
		int addr = mips->alloc_addr(4);
		table->write_addr(addr);
	}
	
	if (function.kind == voidfunc || function.kind == npvoidfunc) {
		fcode << "void " << function.name << "()" << endl;
	}
	else if (function.kind == charfunc || function.kind == npcharfunc) {
		fcode << "char " << function.name << "()" << endl;
	}
	else if (function.kind == intfunc || function.kind == npintfunc) {
		fcode << "int " << function.name << "()" << endl;
	}
	else {
		fcode << "Unexpected error in Code::funcdec" << endl;
	}
}


/*�������� para int paramenter*/
void Code::para(record parameter) {
	parameter.addr = mips->alloc_stack(4);
	table->add(parameter);
	
	if (parameter.kind == charpara) {
		fcode << "para char" << parameter.name << endl;
	}
	else if (parameter.kind == intpara) {
		fcode << "para char" << parameter.name << endl;
	}
	else {
		fcode << "Unexpected error in Code::para" << endl;
	}
}


/*ǿ�Ʒ���*/
void Code::force() {
	//table->pop_level();
	mips->move(29, 30);
	mips->jr(31);
}


/*������� call func() */
record Code::call(record func) {
	record temp;

	mips->push();
	mips->jal(func.name);
	mips->pop();

	if (func.kind == charfunc || func.kind == npcharfunc) {
		temp = new_temp(charvar);
		load(8, func);
		store(8, temp);
	}
	else if (func.kind == intfunc || func.kind == npintfunc) {
		temp = new_temp(intvar);
		load(8, func);
		store(8, temp);
	}
	else {
		temp = func;
	}

	fcode << "call " << func.name << endl;
	return temp;
}


/*�������� push para*/
void Code::para_trans(record para, record expr) {
	load(8, expr);
	mips->sw(8, para.addr, 29);
	fcode << "push " << para.name << endl;
}


/*������� return*/
void Code::ret() {
	force();
	fcode << "return" << endl;
}


/*������� return var */
void Code::ret(record func, record var) {
	load(8, var);
	store(8, func);
	force();
	fcode << "return " << var.name << endl;
}


/*������� scanf var*/
void Code::scan(record var) {
	if (var.kind == intvar) {
		mips->scan_int();
	}
	if (var.kind == charvar) {
		mips->scan_char();
	}
	store(2, var);

	fcode << "scanf " << var.name << endl;
}


/*������� printf var*/
void Code::print(record var) {
	load(4, var);

	if (var.kind == charconst || var.kind == charvar || var.kind == charpara ||
		var.kind == charfunc || var.kind == npcharfunc || var.kind == chararray) {
		mips->print_char();
	}
	else if (var.kind == intconst || var.kind == intvar || var.kind == intpara ||
			 var.kind == intfunc || var.kind == npintfunc || var.kind == intarray) {
		mips->print_int();
	}

	fcode << "printf " << var.name << endl;
}


/*����ַ��� printf output*/
void Code::prints(string text) {
	mips->print_string(text);
	fcode << "printf \"" << text + "\"" << endl;
}


/*��ֵ��� left = right*/
void Code::assign(record left, record right) {
	load(8, right);
	mips->add(9, 8, 0);
	store(9, left);

	fcode << left.name << " = " << right.name << endl;
}


/*ȡֵ*/
void Code::load(int reg, record rec) {
	if (rec.kind == charconst || rec.kind == intconst) {
		mips->li(reg, rec.value);
	}
	else if (rec.kind == intarray || rec.kind == chararray) {
		mips->lw(7, rec.addr, 30);
		mips->lw(reg, 0, 7);
	}
	else if (rec.kind == charfunc || rec.kind == intfunc ||
			 rec.kind == npcharfunc || rec.kind == npintfunc) {
		mips->lw(reg, rec.addr, 0);
	}
	else if (rec.level == 0) {
		mips->lw(reg, rec.addr, 0);
	}
	else {
		mips->lw(reg, rec.addr, 30);
	}
}


/*����*/
void Code::store(int reg, record rec) {
	if (rec.kind == charconst || rec.kind == intconst) {
		//do nothing
	}
	else if (rec.kind == intarray || rec.kind == chararray) {
		mips->lw(7, rec.addr, 30);
		mips->sw(reg, 0, 7);
	}
	else if (rec.kind == charfunc || rec.kind == intfunc ||
			 rec.kind == npcharfunc || rec.kind == npintfunc) {
		mips->sw(reg, rec.addr, 0);
	}
	else if (rec.level == 0) {
		mips->sw(reg, rec.addr, 0);
	}
	else {
		mips->sw(reg, rec.addr, 30);
	}
}


/*�������*/
////////////////////////////////////////////////////////////////////////////////////////////////

/*ȡ�� ^1 = -num
return: �µ���ʱ����*/
record Code::neg(record num) {
	record result = new_temp(intvar);

	load(8, num);
	mips->sub(9, 0, 8);
	store(9, result);

	fcode << result.name << "= -" << num.name << endl;
	return result;
}


/*�ӷ� result = op1 + op2
return: record of the result*/
record Code::plus(record op1, record op2) {
	record result = new_temp(intvar);

	load(8, op1);
	load(9, op2);
	mips->add(10, 8, 9);
	store(10, result);

	fcode << result.name << " = " << op1.name << " + " << op2.name << endl;
	return result;
}


/*���� result = op1 - op2
return: record of the result*/
record Code::minus(record op1, record op2) {
	record result = new_temp(intvar);

	load(8, op1);
	load(9, op2);
	mips->sub(10, 8, 9);
	store(10, result);

	fcode << result.name << " = " << op1.name << " - " << op2.name << endl;
	return result;
}


/*�˷� result = op1 * op2
return: record of the result*/
record Code::mult(record op1, record op2) {
	record result = new_temp(intvar);

	load(8, op1);
	load(9, op2);
	mips->mult(8, 9);
	mips->mflo(10);
	store(10, result);

	fcode << result.name << " = " << op1.name << " * " << op2.name << endl;
	return result;
}


/*���� result = op1 / op2
return: record of the result*/
record Code::div(record op1, record op2) {
	record result = new_temp(intvar);

	load(8, op1);
	load(9, op2);
	mips->div(8, 9);
	mips->mflo(10);
	store(10, result);

	fcode << result.name << " = " << op1.name << " / " << op2.name << endl;
	return result;
}


/*�������� ^1 === array_name[index]*/
record Code::array_call(record array_name, record index) {
	record rec;
	int base;

	if (array_name.kind == chararray) {
		rec = new_temp(chararray);
	}
	else if (array_name.kind == intarray) {
		rec = new_temp(intarray);
	}

	load(8, index);
	mips->li(9, 4);
	mips->mult(8, 9);
	mips->mflo(10);				//$10 = index*4
	base = array_name.addr;		//�������ַ
	mips->li(11, base);
	mips->add(12, 10, 11);		//$12 = real addr
	mips->sw(12, rec.addr, 30);	//rec = real addr

	rec.level = array_name.level;

	fcode << rec.name << " == &" << array_name.name << "[" << index.name << "]" << endl;
	return rec;
}


/*��ת����*/
////////////////////////////////////////////////////////////////////////////////////////////////

/*��һ���ʽ����, ��������ת����Label
���ʽΪ0����Ϊ�٣�����Ϊ��*/
int Code::condition(record single) {
	lnum++;
	load(8, single);
	mips->beq(8, 0, lnum);

	fcode << single.name << " != 0" << endl;
	fcode << "BZ L" << lnum << ": " << endl;
	return lnum;
}

/*���� left == right, ��������ת����Label*/
int Code::equal(record left, record right) {
	lnum++;

	load(8, left);
	load(9, right);
	mips->bne(8, 9, lnum);

	fcode << left.name << " == " << right.name << endl;
	fcode << "BZ L" << lnum << ": " << endl;
	return lnum;
}

/*���� left != right, ��������ת����Label*/
int Code::notequal(record left, record right) {
	lnum++;

	load(8, left);
	load(9, right);
	mips->beq(8, 9, lnum);

	fcode << left.name << " != " << right.name << endl;
	fcode << "BZ L" << lnum << ": " << endl;
	return lnum;
}

/*���� left > right, ��������ת����Label*/
int Code::big(record left, record right) {
	lnum++;

	load(8, left);
	load(9, right);
	mips->ble(8, 9, lnum);

	fcode << left.name << " > " << right.name << endl;
	fcode << "BZ L" << lnum << ": " << endl;
	return lnum;
}

/*���� left >= right, ��������ת����Label*/
int Code::bigequal(record left, record right) {
	lnum++;
	
	load(8, left);
	load(9, right);
	mips->blt(8, 9, lnum);

	fcode << left.name << " >= " << right.name << endl;
	fcode << "BZ L" << lnum << ": " << endl;
	return lnum;
}

/*���� left < right, ��������ת����Label*/
int Code::less(record left, record right) {
	lnum++;
	
	load(8, left);
	load(9, right);
	mips->bge(8, 9, lnum);

	fcode << left.name << " < " << right.name << endl;
	fcode << "BZ L" << lnum << ": " << endl;
	return lnum;
}

/*���� left <= right, ��������ת����Label*/
int Code::lessequal(record left, record right) {
	lnum++;
	
	load(8, left);
	load(9, right);
	mips->bgt(8, 9, lnum);

	fcode << left.name << " <= " << right.name << endl;
	fcode << "BZ L" << lnum << ": " << endl;
	return lnum;
}


/*��������ת����Label*/
int Code::jump() {
	lnum++;
	mips->j(lnum);
	fcode << "goto L" << lnum << ": " << endl;
	return lnum;
}


/*��������ת�����е�Label*/
void Code::jump(int label) {
	mips->j(label);
	fcode << "goto L" << label << ": " << endl;
}
