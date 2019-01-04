#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <iostream>
using namespace std;

class Error {
public:
	Error();

	int errnum;	//´íÎó¼ÆÊı
	
	void err(string message);	//±¨´í
};

#endif