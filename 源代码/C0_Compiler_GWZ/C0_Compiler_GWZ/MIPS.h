#ifndef MIPS_H
#define MIPS_H

#include "struct.h"
#include <iostream>
#include <string>
using namespace std;


class MIPS {
private:
	static string reg[32];			//�Ĵ�����
	static int saved_reg[10];		//����Ĵ���
	static int string_num;			//�ַ������
	static int alloc;				//ȫ�ֵ�ַ
	static int offset;				//ջ��ƫ�Ƶ�ַ

public:
	MIPS();
	void initial ();	//��ʼ��

	//���ú���
	void label (string label_name);		//���ú������
	void label (int num);				//���ñ�׼���
	int  alloc_addr (int size);			//����ȫ�ֵ�ַ
	int  alloc_stack (int size);		//����ջ�ڵ�ַ
	void new_offset ();					//�µ�offset
	void push ();			//�Ĵ�����ջ
	void pop ();			//�ָ��Ĵ���
	void scan_int ();		//��������
	void scan_char ();		//�����ַ�
	void print_int ();		//�������
	void print_char ();		//����ַ�
	void print_string (string text);	//����ַ���

	//����αָ��
	void li (int t1, int num);
	void la (int t1, string label, int t2);
	void move (int t1, int t2);

	//�Ӽ�ָ��
	void add (int t1, int t2, int t3);
	void addi (int t1, int t2, int num);
	void sub (int t1, int t2, int t3);
	void subi (int t1, int t2, int num);

	//�˳�ָ��
	void mfhi (int t1);
	void mflo (int t1);
	void mult (int t1, int t2);
	void div (int t1, int t2);

	//��תָ��
	void j (int label);
	void jr (int t1);
	void jal (string target);

	//��ָ֧��
	void beq (int t1, int t2, int label);
	void bne (int t1, int t2, int label);
	void bge (int t1, int t2, int label);
	void bgt (int t1, int t2, int label);
	void ble (int t1, int t2, int label);
	void blt (int t1, int t2, int label);

	//��ȡָ��(�ô�ָ��)
	void lw (int t1, int num, int t2);
	void sw (int t1, int num, int t2);
	
};

#endif
