#ifndef STRUCT_H
#define STRUCT_H
#include "symbol.h"

//�ʷ���Ԫ
struct token {
	symbol sym;		//��������
	std::string id;	//����
	int num;		//������ֵ
};


//���ű���
struct record {
	std::string name;
	kind kind;		//����
	int value;		//����:ֵ,   ����:Ԫ�ظ���,   ����:��������,    ����:���,    ����:δ����
	int level;		//�������: ȫ����0, ÿ����һ���º���+1, ������������ı��
	int size;		//�ռ��С: �����Ƿ���ֵ�Ŀռ��С; �������ܴ�С
	int addr;		//�ڴ��ַ: �������������ݵ�ַ; ��������ʼ��ַ; �����Ƿ���ֵ�ĵ�ַ(���ݶ�); ������ջ��ƫ�Ƶ�ַ
};


#endif