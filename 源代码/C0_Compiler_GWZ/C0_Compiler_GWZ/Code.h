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
	int lnum;		//Label���
	int tnum;		//��ʱ�������
	Table* table;	//���ű�
	MIPS* mips;		//MIPS������

public:
	Code(Table* table_p, MIPS* mips_p);
	//���ú���
	int  getlabel ();					//������Label
	int  putlabel ();					//������Label,������
	void putlabel (int label);			//�������е�Label
	record new_temp (kind temp_kind);	//�����µ���ʱ����
	
	//�������
	void alloc_add (record r);				//�������
	void funcdec (record function);			//��������
	void para (record parameter);			//��������
	void force ();							//ǿ�Ʒ���
	void ret ();							//�������
	void ret (record func, record name);	//�������,�з���ֵ
	void scan (record name);				//������� scanf name
	void print (record name);				//������� printf name
	void prints (string text);				//����ַ��� printf text
	record call (record func);				//������� call func
	void para_trans (record para, record expr);		//�������� push para
	void assign (record left, record right);		//��ֵ��� left=right
	void load (int reg, record rec);		//ȡֵ
	void store (int reg, record rec);		//����

	//�������
	record neg (record num);					//ȡ��
	record plus (record op1, record op2);		//�ӷ�
	record minus (record op1, record op2);		//����
	record mult (record op1, record op2);		//�˷�
	record div (record op1, record op2);		//����
	record array_call (record array_name, record index);	//�������� ^1 === array_name[index]

	//��ת����
	int  condition (record single);					//��һ���ʽ����, ��������ת����Label
	int  equal (record left, record right);			//���� left == right, ��������ת����Label
	int  notequal (record left, record right);		//���� left != right, ��������ת����Label
	int  big (record left, record right);			//���� left > right, ��������ת����Label
	int  bigequal (record left, record right);		//���� left >= right, ��������ת����Label
	int  less (record left, record right);			//���� left < right, ��������ת����Label
	int  lessequal (record left, record right);		//���� left <= right, ��������ת����Label

	int  jump ();				//��������ת����Label
	void jump (int label);		//��������ת�����е�Label

};

#endif 
