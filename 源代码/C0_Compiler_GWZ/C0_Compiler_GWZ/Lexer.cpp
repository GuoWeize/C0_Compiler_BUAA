#include "Lexer.h"
#include "Error.h"
#include <fstream>

using namespace std;
extern ifstream fin;
extern ofstream fout;

Lexer::Lexer(Error* err_p) {
	Lexer::error = err_p;
	if (!fin.is_open()) {
		cout << "找不到输入文件" << endl;
		cout <<  "(*﹏*)" << endl;
		int i;
		cin >> i;
	}
	ch = ' ';
	line = 1;
	word = 0;
	is_over = 0;
	getch();	//预读字符
}

void Lexer::getch() {
	ch = fin.get();
	if (!fin.eof())
		word++;
	else {
		is_over = 1;
	}
}


//封装的报错函数(包含位置)
void Lexer::has_err(string message) {
	cout << "第" << line << "行," << "第" << word << "个字: ";
	error->err(message);
}



/*词法分析主函数:读取单词*/
token Lexer::getsym() {
	id = "";
	t.id = "";
	t.num = 0;
	t.sym = NULLSYM;

	//跳过空白字符
	while (ch == '\t' || ch == ' ' || ch == '\n') {
		if (ch == '\n') {
			line++;
			word = 0;
		}
		getch();
	}
	
	//保留字和标识符
	if (isalpha(ch) || ch == '_') {
		id += ch;
		if (isalpha(ch)) {
			ch = tolower(ch);
			t.id += ch;
		}
		else {
			t.id += ch;
		}
		getch();
		while (isalnum(ch) || ch == '_') {
			id += ch;
			if (isalpha(ch)) {
				ch = tolower(ch);
				t.id += ch;
			}
			else {
				t.id += ch;
			}
			getch();
		}

		//check reserve word
		if (t.id.compare("const") == 0) { t.sym = CONST; }
		else if (t.id.compare("void") == 0) { t.sym = VOID; }
		else if (t.id.compare("char") == 0) { t.sym = CHAR; }
		else if (t.id.compare("int") == 0) { t.sym = INT; }

		else if (t.id.compare("main") == 0) { t.sym = MAIN; }
		else if (t.id.compare("if") == 0) { t.sym = IF; }
		else if (t.id.compare("else") == 0) { t.sym = ELSE; }
		else if (t.id.compare("switch") == 0) { t.sym = SWITCH; }
		else if (t.id.compare("case") == 0) { t.sym = CASE; }
		else if (t.id.compare("default") == 0) { t.sym = DEFAULT; }
		else if (t.id.compare("while") == 0) { t.sym = WHILE; }
		else if (t.id.compare("return") == 0) { t.sym = RETURN; }
		else if (t.id.compare("scanf") == 0) { t.sym = SCANF; }
		else if (t.id.compare("printf") == 0) { t.sym = PRINTF; }
		else { t.sym = ID; }
		return t;
	}

	//数字(无符号整数)
	if (isdigit(ch)) {
		if (ch == '0') {		//单独处理0
			getch();
			t.sym = ZERO; t.num = 0;
		}
		else {
			id += ch;
			getch();
			while (isdigit(ch)) {
				id += ch;
				getch();
			}
			t.sym = INTEGER;
			t.num = atoi(id.c_str());
		}
		return t;
	}

	//character
	if (ch == '\'') {
		getch();
		char c = ch;
		t.num = c - '*' + 42;
		while (ch != '\n' && ch!='\'') {
			t.id += ch;
			getch();
		}
		if (ch == '\n') {
			line++;
			word = 0;
			has_err("字符不完整，缺少'结尾");
		}
		if (ch == '\'') {
			if (t.id.length() == 1 && (isalnum(c) || c == '_' || c == '+' || c == '-' || c == '*' || c == '/')) {
				t.sym = CHARACTER;
			}
			else {
				if (t.id.length() == 0) { 
					has_err("字符中必须至少包含一个字符，不能为空");
				}
				else if (t.id.length() > 1) { 
					has_err("字符中字符多于1个");
				}
				else { 
					has_err("字符中包含非法字符，只有字母、数字、下划线和四则运算符合法");
				}
			}
		}
		getch();
		return t;
	}

	//string
	if (ch == '"') {
		int flag = 1;
		getch();
		while (ch != '\n' && ch != '\"') {
			if (!(ch >= 32 && ch <= 126)) { 
				has_err("字符串中有非法字符");
				flag = 0;
			}
			t.id += ch;
			getch();
		}
		if (ch == '\n') {
			line++;
			word = 0;
			has_err("字符串不完整，缺少\"结尾");
		}
		if (ch == '\"') {
			if (flag == 1) {
				t.sym = STRING;
			}
		}
		getch();
		return t;
	}

	//!=
	if (ch == '!') {
		getch();
		if (ch == '=') {
			getch();
			t.sym = NOTEQ;
		}
		else {
			has_err("不等号错误，应为!=");
		}
		return t;
	}

	//= ==
	if (ch == '=') {
		getch();
		if (ch == '=') {
			getch();
			t.sym = EQ;
		}
		else {
			t.sym = ASSIGN;
		}
		return t;
	}

	//< <=
	if (ch == '<') {
		getch();
		if (ch == '=') {
			getch();
			t.sym = LESSEQ;
		}
		else if (ch == '>') {
			getch();
			has_err("不等号错误，应为!=");
		}
		else {
			t.sym = LESS;
		}
		return t;
	}

	//> >=
	if (ch == '>') {
		getch();
		if (ch == '=') {
			getch();
			t.sym = BIGEQ;
		}
		else {
			t.sym = BIG;
		}
		return t;
	}

	//各种符号
	if (ch == '+') { getch();	t.sym = PLUS;	return t; }
	if (ch == '-') { getch();	t.sym = MINUS;	return t; }
	if (ch == '*') { getch();	t.sym = STAR;	return t; }
	if (ch == '/') { getch();	t.sym = DIV;	return t; }
	if (ch == '(') { getch();	t.sym = LSMALL;	return t; }
	if (ch == ')') { getch();	t.sym = RSMALL;	return t; }
	if (ch == '[') { getch();	t.sym = LMID;	return t; }
	if (ch == ']') { getch();	t.sym = RMID;	return t; }
	if (ch == '{') { getch();	t.sym = LBIG;	return t; }
	if (ch == '}') { getch();	t.sym = RBIG;	return t; }
	if (ch == ',') { getch();	t.sym = COMMA;	return t; }
	if (ch == ';') { getch();	t.sym = SEMI;	return t; }
	if (ch == ':') { getch();	t.sym = COLON;	return t; }

	else {
		if (is_over==1) {
			has_err("代码不完整，缺少主函数main");
			cout << "Compiling finished." << endl;
			cout << "orz  Sorry, you have " << error->errnum << " Errors." << endl;
			cout << "Start debuging! Good luck!" << endl;
			int i;
			cin >> i;
		}
		has_err("程序不允许的字符");
		return t;
	}

}


void Lexer::all_exit() {
	fout << endl;
	if (error->errnum == 0) {
		fout << "#Compiling finished.#" << endl;
		fout << "#No errors. ^_^ #" << endl;
		exit(1);
	}
	else {
		cout << "Compiling finished." << endl;
		cout << "orz  Sorry, you have " << error->errnum << " Errors." << endl;
		cout << "Start debuging! Good luck!" << endl;
		int i;
		cin >> i;
	}
}