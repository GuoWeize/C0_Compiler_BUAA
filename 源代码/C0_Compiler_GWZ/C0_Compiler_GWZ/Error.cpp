#include "Error.h"
#include <iostream>
#include <fstream>
using namespace std;

Error::Error() {
	errnum = 0;
}


void Error::err(string message) {
	cout << message << endl;
	errnum++;
}
