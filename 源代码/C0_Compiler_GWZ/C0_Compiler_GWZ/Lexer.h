#ifndef LEXER_H
#define LEXER_H

#include "struct.h"
#include "Error.h"
#include <iostream>
#include <string>
using namespace std;

//�ʷ�����
class Lexer {
private:
	Error* error;	//���������
	char ch;		//������ַ�
	string id;		//�����Ĵʷ���Ԫ��(��ת����Сд)
	token t;		//�����Ĵʷ���Ԫ
	int line;		//�м���
	int word;		//ÿ�д����ַ�����
	int is_over;	//�ж��Ƿ����
	void getch();	//��ȡһ���ַ�

public:
	Lexer::Lexer(Error* err_p);
	token getsym();		//�ʷ�����������:��ȡ����
	void all_exit();	//�˳�����
	void has_err(string message);	//��װ�ı�����(����λ��)
	
};

#endif