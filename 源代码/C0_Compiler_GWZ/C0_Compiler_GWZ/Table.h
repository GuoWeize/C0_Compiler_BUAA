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
	static int num;			//�������
	vector<record> global;	//ȫ�ֱ�����¼
	vector<record> t;		//���ű��¼
	vector<record>::iterator iter;	//������

public:
	Table();
	int level;		//����ʱ��ǰ�������, 0��ȫ��
	bool is_found (string name);			//��ȫ�ֻ����ݱ�, ��ʶ���Ƿ��ظ�����
	bool is_found_global (string name);		//��ȫ�ֱ�, ��ʶ���Ƿ��ظ�����
	record find (string name);			//��ȫ�ֺ����ݱ�, ���ؼ�¼  δ�ҵ�����addrΪ-1�ļ�¼
	record find_global (string name);	//��ȫ�ֱ�, ���ؼ�¼  δ�ҵ�����addrΪ-1�ļ�¼
	record last_global ();				//ȫ�ֱ������һ��������¼
	record find_para (int func_level, int para_index);	//���Ҳ���
	void add (record r);				//�����ݱ����
	void add_global(record r);			//��ȫ�ֱ����
	void write_value (int value);		//����value
	void write_addr (int addr);			//����addr
	void new_level ();					//�������num+1, ����ֵ��level

};

#endif 
