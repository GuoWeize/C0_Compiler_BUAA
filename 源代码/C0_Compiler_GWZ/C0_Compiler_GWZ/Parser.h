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
	Lexer* lexer;	//�ʷ���������
	Table* table;	//���ű�
	Code*  code;	//�м����������

	token t;		//����ĵ���
	record r;		//����ķ��ű���
	int is_forward;	//Ԥ������
	token forward1, forward2;	//Ԥ���ĵ���
	vector<symbol> all_sym;		//�������������
	void read();			//����һ������
	token forwardread();	//Ԥ��һ������
	void skip();			//����
	void skip(vector<symbol> fsys);	//����
	void newrecord();		//���ü�¼r
	bool is_same_kind (kind const_kind, kind other);		//�жϳ��������Ƿ�һ��
	bool is_return (kind function_kind, kind return_kind);	//�жϷ��������Ƿ�һ��
	bool is_para (kind para_kind, kind expr_kind);			//�жϲ��������Ƿ�һ��

public:
	Parser(Lexer* lexer, Table* table_p, Code* code_p);

	//С���﷨��Ŀ
	int integer ();			//����(�з��ź�0)
	record constant ();		//���泣��
	void arraydec ();		//���鶨�� ��[�� Integer ��]��
	record arraycall (record array_name);	//�������� ��[�� <���ʽ> ��]��
	int condition ();		//����
	record factor ();		//����
	record term ();			//��
	record expression ();	//���ʽ
	int formalpara ();		//�βα�
	void actualpara (record function);	//ʵ�α�
	
	//�����Ŀ
	void constdec ();		//��������
	void constdef ();		//��������
	void vardec ();			//��������
	void vardef ();			//��������
	void funcdef ();		//��������
	void statement ();		//�������
	void stats ();			//�������
	record funcall ();		//��������
	void assignstat ();		//��ֵ���
	void ifstat ();			//if���
	void whilestat ();		//while���
	void casestat (record switch_on, int end_label);	//case���
	void defaultstat ();	//default���
	void switchstat ();		//switch���
	void returnstat ();		//return���
	void scanfstat ();		//����Input
	void printfstat ();		//���Output
	
	//���򲿷�
	void compoundstats ();	//�������
	void program ();		//�﷨�������,��ʼ����
	
};

#endif
