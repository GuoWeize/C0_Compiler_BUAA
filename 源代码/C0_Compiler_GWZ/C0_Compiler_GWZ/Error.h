#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <iostream>
using namespace std;

class Error {
public:
	Error();

	int errnum;	//�������
	
	void err(string message);	//����
};

#endif