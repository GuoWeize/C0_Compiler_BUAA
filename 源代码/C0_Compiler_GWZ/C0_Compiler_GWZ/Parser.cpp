#include "Parser.h"
#include "Lexer.h"
#include "Error.h"
#include <fstream>
#include <algorithm>

using namespace std;
extern ofstream fout;


Parser::Parser(Lexer* lexer_p, Table* table_p, Code* code_p) {
	lexer = lexer_p;
	table = table_p;
	code = code_p;

	is_forward = 0;
	token temp;
	temp.id = "";
	temp.num = 0;
	temp.sym = NULLSYM;
	forward1, forward2 = temp;

	read();

	symbol all_array[30] = {
		SEMI,LBIG,RBIG,LMID,RMID,LSMALL,RSMALL,ASSIGN,COMMA,COLON,
		ID,IF,SWITCH,WHILE,SCANF,PRINTF,RETURN,VOID,CHAR,INT,CONST
	};

	for (int i = 0; i < 15; i++) {
		all_sym.push_back(all_array[i]);
	}
}

/*读入一个单词*/
void Parser::read () {
	if (is_forward == 0) {
		t = lexer->getsym();
		return;
	}
	if (is_forward == 1) {
		t = forward1;
		is_forward = 0;
		return;
	}
	if (is_forward == 2) {
		t = forward1;
		forward1 = forward2;
		is_forward = 1;
		return;
	}
	else {
		lexer->has_err("预读单词过多");
		t = forward1;
		forward1 = forward2;
		is_forward = 1;
		return;
	}
}

/*预读一个单词*/
token Parser::forwardread () {
	if (is_forward == 0) {
		forward1 = lexer->getsym();
		is_forward = 1;
		return forward1;
	}
	if (is_forward == 1) {
		forward2 = lexer->getsym();
		is_forward = 2;
		return forward2;
	}
	if (is_forward == 2) {
		return forward2;
	}
	else {
		lexer->has_err("预读单词过多");
		return forward2;
	}
}

/*跳读*/
void Parser::skip() {
	while (t.sym!=SEMI && t.sym!=RBIG) {
		read();
	}
	read();
}

/*跳读*/
void Parser::skip (vector<symbol> fsys) {
	vector<symbol>::iterator ret;
	while (true) {
		ret = find(fsys.begin(), fsys.end(), t.sym);
		if (ret != fsys.end()) {
			return;
		}
		read();
	}
}

/*重置记录r*/
void Parser::newrecord() {
	r.addr = r.level = r.value = r.size = 0;
	r.kind = npvoidfunc;
	r.name = "";
}

/*判断常量类型是否一致*/
bool Parser::is_same_kind(kind const_kind, kind other) {
	if (const_kind == intconst) {
		if (other == intconst || other == intfunc || other == npintfunc ||
			other == intvar || other == intpara || other == intarray) {
			return true;
		}
	}
	if (const_kind == charconst) {
		if (other == charconst || other == charfunc || other == npcharfunc ||
			other == charvar || other == charpara || other == chararray) {
			return true;
		}
	}
	return false;
}

/*判断返回类型是否一致*/
bool Parser::is_return(kind function_kind, kind return_kind) {
	if (function_kind == charfunc || function_kind == npcharfunc) {
		if (return_kind == charconst || return_kind == charfunc || return_kind == npcharfunc ||
			return_kind == charvar || return_kind == charpara || return_kind == chararray) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (function_kind == intfunc || function_kind == npintfunc) {
		if (return_kind == intconst || return_kind == intfunc || return_kind == npintfunc ||
			return_kind == intvar || return_kind == intpara || return_kind == intarray) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}


/*判断参数类型是否一致*/
bool Parser::is_para(kind para_kind, kind expr_kind) {
	if (para_kind == charpara) {
		if (expr_kind == charconst || expr_kind == charfunc || expr_kind == npcharfunc ||
			expr_kind == charvar || expr_kind == charpara || expr_kind == chararray) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (para_kind == intpara) {
		if (expr_kind == intconst || expr_kind == intfunc || expr_kind == npintfunc ||
			expr_kind == intvar || expr_kind == intpara || expr_kind == intarray) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}


//预处理部分
//////////////////////////////////////////////////////////////////////////////////////

/*<整数> ::= [+|-]<无符号整数> | 0
return 整数数值*/
int Parser::integer () {
	int base = 1;

	if (t.sym == PLUS || t.sym == MINUS) {
		if (t.sym == MINUS) {
			base = -1;
		}
		read();
		if (t.sym == INTEGER) {
			base = base * t.num;
			read();
			return base;
		}
		else {
			lexer->has_err("缺少无符号整数");
			skip(all_sym);
		}
	}
	else if (t.sym == INTEGER) {
		base = base * t.num;
		read();
		return base;
	}
	else if (t.sym == ZERO) {
		read();
		return 0;
	}
	else {
		lexer->has_err("缺少整数");
		skip(all_sym);
	}

	return 0;
}


/* <常量> ::= <整数> | Character
return: record of the constant */
record Parser::constant () {
	record rec;
	rec.size = 4;

	if (t.sym == INTEGER || t.sym == ZERO || t.sym == PLUS || t.sym == MINUS) {
		rec.value = integer();
		rec.name = to_string(static_cast<long long>(rec.value));
		rec.kind = intconst;
	}
	else if (t.sym == CHARACTER) {
		rec.value = t.num;
		rec.name = t.id;
		rec.kind = charconst;
		read();
	}
	else {
		lexer->has_err("缺少常量");
		skip(all_sym);
	}

	return rec;
}


/*<数组定义> ::= ‘[’ Integer ‘]’ */
void Parser::arraydec () {
	if (r.kind == intvar) {
		r.kind = intarray;
	}
	else if (r.kind == charvar) {
		r.kind = chararray;
	}

	if (t.sym == LMID) {
		read();
	}
	else {
		lexer->has_err("缺少左中括号");
		skip(all_sym);
	}

	if (t.sym == INTEGER) {
		r.value = t.num;
		r.size = t.num * 4;
		read();
	}
	else {
		lexer->has_err("缺少无符号整数");
		skip(all_sym);
	}

	if (t.sym == RMID) {
		read();
	}
	else {
		lexer->has_err("缺少右中括号");
		skip(all_sym);
	}
	return;
}


/*<数组引用> ::= ‘[’ <表达式> ‘]’
array_name: 数组记录
return: 数组引用的结果记录*/
record Parser::arraycall (record array_name) {
	if (t.sym == LMID) {
		read();
	}
	else {
		lexer->has_err("缺少左中括号");
		skip(all_sym);
	}

	if (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER ||
		t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {

		record expr = expression();
		if (expr.kind == intconst) {		//数组越界
			if (expr.value < 0 || expr.value >= array_name.value) {
				lexer->has_err("数组访问越界");
			}
		}
		else if (expr.kind == intpara || expr.kind == intvar || expr.kind == intconst ||
			expr.kind == intfunc || expr.kind == npintfunc || expr.kind == intarray) {
		}
		else {
			lexer->has_err("数组引用下标非法,必须为整型值");
		}

		if (t.sym == RMID) {
			read();
			return code->array_call(array_name, expr);
		}
		else {
			lexer->has_err("缺少右中括号");
			skip(all_sym);
		}
	}

	else {
		lexer->has_err("缺少表达式");
		skip();
	}

	return array_name;
}


/*<因子> ::= Id | Id<数组引用> | ‘(’<表达式>‘)’ | <常量> | <有返回值函数调用语句>
return: record of the result*/
record Parser::factor () {
	record result;

	switch (t.sym) {
		case ID:			// Id | Id<数组引用> | <有返回值函数调用语句>
		{
			record rec = table->find(t.id);
			if (rec.addr == -1) {
				lexer->has_err("符号:" + t.id + "未定义");
			}
			else {
				if (rec.kind == chararray || rec.kind == intarray) {
					read();
					result = arraycall(rec);
				}
				else if (rec.kind == charfunc || rec.kind == intfunc ||
						 rec.kind == npcharfunc || rec.kind == npintfunc) {
					result = funcall();
				}
				else if (rec.kind == charconst || rec.kind == charvar || rec.kind == charpara ||
						 rec.kind == intconst || rec.kind == intvar || rec.kind == intpara) {
					read();
					result = rec;
				}
				else {
					lexer->has_err("不是可以引用的值");
				}
			}
			return result;
		}
		case PLUS:			// <常量>
			result = constant();
			return result;
		case MINUS:			// <常量>
			result = constant();
			return result;
		case INTEGER:		// <常量>
			result = constant();
			return result;
		case ZERO:			// <常量>
			result = constant();
			return result;
		case CHARACTER:		// <常量>
			result = constant();
			return result;
		case LSMALL:		// ‘(’<表达式>‘)’
			read();
			result = expression();
			if (t.sym == RSMALL) {
				read();
			}
			else {
				lexer->has_err("缺少右小括号");
				skip(all_sym);
				break;
			}
			return result;
		default:			//Error!
			lexer->has_err("缺少运算对象");
			skip(all_sym);
	}
	return result;
}


/*<项> ::= <因子> { <乘法运算符> <因子> }
return: record of the result*/
record Parser::term () {
	record op1, op2, result;
	bool isdiv = true;

	if (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER ||
		t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {
		op1 = factor();
		result = op1;
	}
	else {
		lexer->has_err("缺少运算对象");
		skip(all_sym);
	}

	while (t.sym == STAR || t.sym == DIV) {
		if (t.sym == STAR) {
			isdiv = false;
		}
		else {
			isdiv = true;
		}

		read();
		if (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER ||
			t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {
			op1 = result;
			op2 = factor();

			if (isdiv) {
				result = code->div(op1, op2);
			}
			else {
				result = code->mult(op1, op2);
			}
		}
		else {
			lexer->has_err("缺少运算对象");
			skip(all_sym);
		}
	}

	return result;
}


/*<表达式> ::= [＋|－] <项> { <加法运算符> <项> }
return: record of the result*/
record Parser::expression () {
	record op1, op2, result;
	bool isplus = true;
	bool positive = true;

	if (t.sym == PLUS || t.sym == MINUS) {
		forwardread();
		if (forward1.sym == INTEGER) {
			//do nothing
		}
		else {
			if (t.sym == PLUS) {
				read();
			}
			else if (t.sym == MINUS) {
				read();
				positive = false;
			}
		}
	}

	if (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER ||
		t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {
		op1 = term();
		result = op1;
	}
	else {
		lexer->has_err("缺少运算对象");
		skip(all_sym);
	}

	while (t.sym == PLUS || t.sym == MINUS) {
		if (t.sym == PLUS) {
			isplus = true;
		}
		else if (t.sym == MINUS) {
			isplus = false;
		}
		read();
		if (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER ||
			t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {
			op1 = result;
			op2 = term();
			if (isplus) {
				result = code->plus(op1, op2);
			}
			else {
				result = code->minus(op1, op2);
			}
		}
		else {
			lexer->has_err("缺少运算对象");
			skip(all_sym);
		}
	}

	if (!positive) {
		result = code->neg(result);
	}

	return result;
}


/*<条件> ::= <表达式> <关系运算符> <表达式> | <表达式>    表达式为0条件为假，否则为真 */
int Parser::condition () {
	record left, right;
	symbol op = NULLSYM;

	if (t.sym == ID || t.sym == INTEGER || t.sym == CHARACTER || t.sym == LSMALL ||
		t.sym == PLUS || t.sym == MINUS || t.sym == ZERO) {
		left = expression();

		if (t.sym == EQ || t.sym == BIG || t.sym == LESS ||
			t.sym == BIGEQ ||t.sym == LESSEQ || t.sym == NOTEQ) {
			op = t.sym;
			read();
			right = expression();

			if (op == EQ) {
				return code->equal(left, right);
			}
			else if (op == BIG) {
				return code->big(left, right);
			}
			else if (op == LESS) {
				return code->less(left, right);
			}
			else if (op == BIGEQ) {
				return code->bigequal(left, right);
			}
			else if (op == LESSEQ) {
				return code->lessequal(left, right);
			}
			else if (op == NOTEQ) {
				return code->notequal(left, right);
			}
		}
		else {
			return code->condition(left);
		}
	}
	else {
		lexer->has_err("缺少表达式");
		skip();
	}

	return -1;
}


/*<参数表> ::= <类型标识符> Id { , <类型标识符> Id }
rerurn 参数个数*/
int Parser::formalpara () {
	int paranum = 0;
	newrecord();

	while (t.sym == INT || t.sym == CHAR) {
		r.size = 4;
		if (t.sym == INT) {
			r.kind = intpara;
		}
		else if (t.sym == CHAR) {
			r.kind = charpara;
		}
		read();

		if (t.sym == ID) {
			r.name = t.id;
			read();
			r.value = paranum + 1;
			code->para(r);
			newrecord();
			paranum++;

			if (t.sym == COMMA) {
				read();
			}
			else {
				return paranum;
			}
		}
		else {
			lexer->has_err("缺少标识符");
			skip();
			return paranum;
		}
	}

	if (t.sym == VOID) {
		lexer->has_err("声明类型错误，不可以为void");
		skip();
	}
	else if (paranum == 0) {
		lexer->has_err("缺少参数");
		skip();
	}
	return paranum;
}


/*<值参数表> ::= <表达式> { , <表达式> }
paranum: number of parameters*/
void Parser::actualpara (record function) {
	int get_num = 0;
	record para, expr;
	vector<record> all_para;
	vector<record> all_expr;

	while (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER ||
		   t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {
		if (get_num == function.value) {
			lexer->has_err("传入参数过多");
			skip();
			return;
		}

		expr = expression();
		para = table->find_para(function.level, get_num+1);
		all_para.push_back(para);

		if (is_para(para.kind, expr.kind)) {
		}
		else {
			lexer->has_err("参数类型不符");
		}

		all_expr.push_back(expr);
		get_num++;
		if (t.sym == COMMA) {
			read();
		}
		else {
			if (get_num != function.value) {
				lexer->has_err("传入参数过少");
				skip();
			}
			else {
				//正常传参
				for (int i = 0; i < function.value; i++) {
					code->para_trans(all_para[i], all_expr[i]);
				}
			}
			return;
		}
	}

	lexer->has_err("传入参数过少");
	skip();
	return;
}


//声明部分
//////////////////////////////////////////////////////////////////////////////////////


/*<常量说明> ::= const <常量定义>; { const <常量定义>; } */
void Parser::constdec () {
	int constdefnum = 0;

	while (t.sym == CONST) {
		read();
		newrecord();
		constdef();
		if (t.sym == SEMI) {
			read();
		}
		else {
			lexer->has_err("缺少分号");
			skip(all_sym);
		}
		constdefnum++;
	}
	if (constdefnum>0) {
		//fout << "常量声明语句" << endl;
	}
	else {
		lexer->has_err("常量声明要以const开始");
		skip();
	}
	return;
}


/*<常量定义> ::= int Id＝<整数> { , Id ＝ <整数>} | char Id ＝ Character { , Id ＝ Character } */
void Parser::constdef () {
	int constnum = 0;
	newrecord();
	r.size = 4;

	switch (t.sym) {
		case INT:
			read();
			r.kind = intconst;
			while (t.sym == ID) {
				if (table->is_found(t.id)) {
					lexer->has_err("标识符重复定义");
					skip();
					return;
				}
				else {
					r.name = t.id;
					read();
					if (t.sym == ASSIGN) {
						read();
					}
					else {
						lexer->has_err("缺少赋值符号");
						skip(all_sym);
					}
					r.value = integer();
					constnum++;

					if (t.sym == COMMA) {
						read();
						table->add(r);
					}
					else {
						break;
					}
				}
			}
			break;

		case CHAR:
			read();
			r.kind = charconst;
			while (t.sym == ID) {
				if (table->is_found(t.id)) {
					lexer->has_err("标识符重复定义");
					skip();
					return;
				}
				else {
					r.name = t.id;
					read();
					if (t.sym == ASSIGN) {
						read();
					}
					else {
						lexer->has_err("缺少赋值符号");
						skip(all_sym);
					}

					if (t.sym == CHARACTER) {
						r.value = t.num;
						read();
						constnum++;
						if (t.sym == COMMA) {
							read();
							table->add(r);
						}
						else {
							break;
						}
					}
					else {
						lexer->has_err("缺少字符");
						skip();
						return;
					}

				}
			}
			break;

		default:
			lexer->has_err("缺少类型，假定为int");
	}

	if (constnum > 0) {
		table->add(r);
		return;
	}
	else {
		newrecord();
		lexer->has_err("缺少常量定义");
		skip();
		return;
	}
}


/*<变量说明> ::= <变量定义>;{<变量定义>;} */
void Parser::vardec () {
	int vardefnum = 0;

	while (t.sym == INT || t.sym == CHAR) {
		newrecord();
		vardef();
		if (t.sym == SEMI) {
			read();
		}
		else {
			lexer->has_err("缺少分号");
			skip(all_sym);
		}
		vardefnum++;
	}
	if (vardefnum>0) {
		return;
	}
	else {
		lexer->has_err("缺少类型，假定为int");
	}
}


/*<变量定义> ::= (int | char) ( Id [<数组定义>] ) { ,( Id [<数组定义>] ) } */
void Parser::vardef () {
	int varnum = 0;
	r.size = 4;

	if (t.sym == INT || t.sym == CHAR) {
		if (t.sym == INT) {
			r.kind = intvar;
		}
		else if (t.sym == CHAR) {
			r.kind = charvar;
		}

		read();
		while (t.sym == ID) {
			if (table->is_found(t.id)) {
				lexer->has_err("标识符重复定义");
				skip();
				return;
			}
			else {
				r.name = t.id;
				read();
				if (t.sym == LMID) {
					arraydec();
				}
				varnum++;

				if (t.sym == COMMA) {
					read();
					code->alloc_add(r);
					
					r.size = 4;
					if (r.kind == intarray || r.kind == intvar) {
						r.kind = intvar;
					}
					else if (r.kind == chararray || r.kind == charvar) {
						r.kind = charvar;
					}
				}
				else {
					break;
				}
			}
		}

		if (varnum>0) {
			code->alloc_add(r);
			return;
		}
		else {
			lexer->has_err("缺少变量定义");
			skip();
			return;
		}

	}
	else {
		lexer->has_err("缺少类型，假定为int");
		skip();
		return;
	}
}


/*<函数定义> ::= (int | char | void) Id ‘(’<参数>‘)’ ‘{’<复合语句>‘}’
| (int | char | void) Id ‘{’ <复合语句> ‘}’ */
void Parser::funcdef () {
	int para_num = 0;
	newrecord();

	if (t.sym == INT || t.sym == CHAR || t.sym == VOID) {
		
		if (t.sym == INT) {			//先初始化为无参数
			r.kind = npintfunc;
		}
		else if (t.sym == CHAR) {
			r.kind = npcharfunc;
		}
		else if (t.sym == VOID) {
			r.kind = npvoidfunc;
		}

		read();
		if (t.sym == ID) {
			if (table->is_found_global(t.id)) {
				lexer->has_err("标识符重复定义");
				skip();
				return;
			}
			else {
				//正常定义
				r.name = t.id;
				read();

				//有参数
				if (t.sym == LSMALL) {
					read();
					//类型判断
					if (r.kind == npvoidfunc) {
						r.kind = voidfunc;
					}
					else if (r.kind == npcharfunc) {
						r.kind = charfunc;
					}
					else if (r.kind == npintfunc) {
						r.kind = intfunc;
					}

					r.size= 4;
					table->new_level();
					table->add_global(r);
					code->funcdec(r);

					para_num = formalpara();
					table->write_value(para_num);
					
					if (t.sym == RSMALL) {
						read();
					}
					else {
						lexer->has_err("缺少右小括号");
						skip(all_sym);
					}
				}

				//无参数
				else {
					r.size = 0;
					r.value = 0;
					r.addr = 0;

					table->new_level();
					table->add_global(r);
					code->funcdec(r);
				}

				//声明体:复合语句
				if (t.sym == LBIG) {
					read();
				}
				else {
					lexer->has_err("缺少左大括号");
					skip(all_sym);
				}
				compoundstats();
				if (t.sym == RBIG) {
					read();
				}
				else {
					lexer->has_err("缺少右大括号");
					skip(all_sym);
				}
				//fout << "函数声明语句" << endl;
				code->force();
				return;

			}//结束正常定义
			
		}
		else {
			lexer->has_err("缺少标识符");
			skip();
			return;
		}
	}
	else {
		lexer->has_err("缺少类型，假定为void");
	}
}


//语句部分
//////////////////////////////////////////////////////////////////////////////////////


/*<语句> ::= <条件语句>｜<循环语句> | ‘{’ {<语句>} ‘}’｜<有返回值函数调用语句> ;| <无返回值函数调用语句> ;
｜<赋值语句> ;｜<读语句> ;｜<写语句> ;｜ ; | <情况语句>｜<返回语句> ; */
void Parser::statement () {
	switch (t.sym) {
		case IF:			//<条件语句>
			ifstat();
			return;
		case WHILE:			//<循环语句>
			whilestat();
			return;
		case LBIG:			//‘{’ {<语句>} ‘}’
			read();
			stats();
			if (t.sym == RBIG) {
				read();
				return;
			}
			else {
				lexer->has_err("缺少右大括号");
				skip(all_sym);
			}
			break;
		case SCANF:			//<读语句> ;
			scanfstat();
			if (t.sym == SEMI) {
				read();
				return;
			}
			else {
				lexer->has_err("缺少分号");
				skip(all_sym);
			}
			break;
		case PRINTF:		//<写语句> ;
			printfstat();
			if (t.sym == SEMI) {
				read();
				return;
			}
			else {
				lexer->has_err("缺少分号");
				skip(all_sym);
			}
			break;
		case SEMI:			// ;
			read();
			return;
		case SWITCH:		//<情况语句>
			switchstat();
			return;
		case RETURN:		//<返回语句> ;
			returnstat();
			if (t.sym == SEMI) {
				read();
				return;
			}
			else {
				lexer->has_err("缺少分号");
				skip(all_sym);
			}
			break;
		case ID:			//<函数调用> ; | <赋值语句> ;
		{
			record rec = table->find(t.id);
			if (rec.addr == -1) {
				//lexer->has_err("符号:" + t.id + "未定义");
				skip();
				return;
			}
			else {
				if (rec.kind == voidfunc || rec.kind == charfunc || rec.kind == intfunc ||
					rec.kind == npvoidfunc || rec.kind == npcharfunc || rec.kind == npintfunc) {
					funcall();
					if (t.sym == SEMI) {
						read();
						return;
					}
					else {
						lexer->has_err("缺少分号");
						skip(all_sym);
					}
				}
				else {
					assignstat();
					if (t.sym == SEMI) {
						read();
						return;
					}
					else {
						lexer->has_err("缺少分号");
						skip(all_sym);
					}
				}
			}
			return;
		}
		default:
			lexer->has_err("缺少语句");
			skip();
			return;
	}
}


/*<语句列> ::= {<语句>} */
void Parser::stats () {
	while (t.sym == IF || t.sym == WHILE || t.sym == LBIG || t.sym == SCANF || t.sym == PRINTF ||
		   t.sym == SEMI || t.sym == SWITCH || t.sym == RETURN || t.sym == ID) {
		statement();
	}
	return;
}


/*<函数调用语句> ::= Id‘(’<值参数表>‘)’ | Id */
record Parser::funcall () {
	vector<record> allpara;
	record func = table->find_global(t.id);

	if (t.sym == ID) {
		read();
		if (func.addr == -1) {
			lexer->has_err("符号:" + t.id + "未定义");
			skip();
			return func;
		}
		else {
			//无参数
			if (func.kind == npvoidfunc || func.kind == npcharfunc || func.kind == npintfunc) {
				//fout << "函数调用语句" << endl;
				return code->call(func);
			}

			//有参数
			else if (func.kind == voidfunc || func.kind == charfunc || func.kind == intfunc) {
				if (t.sym == LSMALL) {
					read();
				}
				else {
					lexer->has_err("缺少左小括号");
					skip(all_sym);
				}

				actualpara(func);
				if (t.sym == RSMALL) {
					read();
				}
				else {
					lexer->has_err("缺少右小括号");
					skip(all_sym);
				}
				//fout << "函数调用语句" << endl;
				return code->call(func);

			}
			else {
				lexer->has_err("函数调用判断错误");
			}
		}
	}
	else {
		lexer->has_err("缺少标识符");
		skip();
		return func;
	}

	return func;
}


/*<赋值语句> ::= Id [<数组引用>] = <表达式> */
void Parser::assignstat () {
	record left, right;

	if (t.sym == ID) {
		record left = table->find(t.id);
		read();

		if (left.addr == -1) {
			lexer->has_err("符号:" + t.id + "未定义");
			skip();
			return;
		}
		else {
			if (left.kind == charvar || left.kind == charpara||
				left.kind == intvar || left.kind == intpara) {
				//do nothing
			}
			else if (left.kind == chararray || left.kind == intarray) {
				left = arraycall(left);
			}
			else {
				lexer->has_err("不是可以被赋值的左值");
			}
		}

		if (t.sym == ASSIGN) {
			read();
		}
		else {
			lexer->has_err("缺少赋值符号");
			skip(all_sym);
		}

		right = expression();
		if ((right.kind == intconst || right.kind == intvar || right.kind == intarray || right.kind == intpara || right.kind == intfunc || right.kind == npintfunc) &&
			(left.kind == charvar || left.kind == charpara || left.kind == chararray)) {
			lexer->has_err("不能将整型赋值给字符型");
		}
		code->assign(left, right);
		//fout << "赋值语句" << endl;
		return;

	}
	else {
		lexer->has_err("缺少标识符");
		skip();
		return;
	}
}


/*<条件语句> ::= if ‘(’<条件>‘)’ <语句> else <语句> */
void Parser::ifstat () {
	int label1, label2;

	if (t.sym == IF) {
		read();
		if (t.sym == LSMALL) {
			read();
		}
		else {
			lexer->has_err("缺少左小括号");
			skip(all_sym);
		}

		label1 = condition();
		if (t.sym == RSMALL) {
			read();
		}
		else {
			lexer->has_err("缺少右小括号");
			skip(all_sym);
		}

		statement();
		label2 = code->jump();
		if (t.sym == ELSE) {
			read();
		}
		else {
			lexer->has_err("缺少else");
			skip();
			return;
		}
		code->putlabel(label1);
		statement();
		code->putlabel(label2);
		//fout << "条件语句" << endl;
		return;

	}
	else {
		lexer->has_err("缺少关键字");
		skip();
		return;
	}
}


/*<循环语句> ::= while ‘(’<条件>‘)’ <语句> */
void Parser::whilestat () {
	int label1, label2;

	if (t.sym == WHILE) {
		read();

		label1 = code->putlabel();
		if (t.sym == LSMALL) {
			read();
		}
		else {
			lexer->has_err("缺少左小括号");
			skip(all_sym);
		}

		label2 = condition();
		if (t.sym == RSMALL) {
			read();
		}
		else {
			lexer->has_err("缺少右小括号");
			skip(all_sym);
		}
		statement();
		code->jump(label1);
		code->putlabel(label2);
		//fout << "循环语句" << endl;
		return;

	}
	else {
		lexer->has_err("缺少关键字");
		skip();
		return;
	}
}


/*<情况表> ::= case <常量> : <语句> { case  <常量> : <语句>}
end_label: label of the end of switch*/
void Parser::casestat (record switch_on, int end_label) {
	record const_rec;
	int casenum = 0;
	int label = 0;

	while (t.sym == CASE) {
		read();
		const_rec = constant();
		if ( is_same_kind(const_rec.kind, switch_on.kind) ) {
		}
		else {
			lexer->has_err("条件数据类型不符");
		}

		if (t.sym == COLON) {
			read();
		}
		else {
			lexer->has_err("缺少冒号");
			skip(all_sym);
		}
		label = code->equal(switch_on, const_rec);
		statement();
		code->jump(end_label);
		code->putlabel(label);
		casenum++;
		continue;

	}

	if (casenum>0) {
		return;
	}
	else {
		lexer->has_err("缺少情况case");
		skip();
		return;
	}

}


/*<缺省>  ::= default : <语句> */
void Parser::defaultstat () {
	if (t.sym == DEFAULT) {
		read();
	}
	else {
		lexer->has_err("缺少default");
		skip();
		return;
	}

	if (t.sym == COLON) {
		read();
	}
	else {
		lexer->has_err("缺少冒号");
		skip(all_sym);
	}
	statement();
	return;
}


/*<情况语句> ::= switch ‘(’<表达式>‘)’ ‘{’<情况表> [<缺省>]‘}’ */
void Parser::switchstat () {
	record expr;
	int end_label;

	if (t.sym == SWITCH) {
		read();
		if (t.sym == LSMALL) {
			read();
		}
		else {
			lexer->has_err("缺少左小括号");
			skip(all_sym);
		}
		{
			expr = expression();
			if (t.sym == RSMALL) {
				read();
			}
			else {
				lexer->has_err("缺少右小括号");
				skip(all_sym);
			}
			{
				end_label = code->getlabel();
				if (t.sym == LBIG) {
					read();
				}
				else {
					lexer->has_err("缺少左大括号");
					skip(all_sym);
				}
				casestat(expr, end_label);
				if (t.sym == DEFAULT) {
					defaultstat();
				}
				if (t.sym == RBIG) {
					read();
				}
				else {
					lexer->has_err("缺少右大括号");
					skip(all_sym);
				}
				code->putlabel(end_label);
				//fout << "情况语句" << endl;
				return;
			}
		}
	}
	else {
		lexer->has_err("缺少switch");
		skip();
		return;
	}
}


/*<返回语句> ::= return [ ‘(’<表达式>‘)’ ] */
void Parser::returnstat () {
	record func = table->last_global();
	record ret;

	if (t.sym == RETURN) {
		read();
		if (t.sym == LSMALL) {
			read();
			ret = expression();

			if (is_return(func.kind, ret.kind)) {
				if (t.sym == RSMALL) {
					read();
				}
				else {
					lexer->has_err("缺少右小括号");
					skip(all_sym);
				}
				code->ret(func, ret);
				//fout << "返回语句" << endl;
				return;
			}
			else {
				lexer->has_err("返回值类型不符");
			}
		}
		else {
			code->ret();
			//fout << "返回语句" << endl;
		}
	}
	else {
		lexer->has_err("缺少retrun");
		skip();
		return;
	}
}


/*<读语句> ::= scanf ‘(’ Id {, Id} ‘)’ */
void Parser::scanfstat () {
	record rec;

	if (t.sym == SCANF) {
		read();
		if (t.sym == LSMALL) {
			read();
		}
		else {
			lexer->has_err("缺少左小括号");
			skip(all_sym);
		}

		if (t.sym == ID) {
			rec = table->find(t.id);
			if (rec.addr == -1) {
				lexer->has_err("符号:" + t.id + "未定义");
				skip();
				return;
			}
			else {
				if (rec.kind == charvar || rec.kind == intvar) {
					read();
					code->scan(rec);

					while (t.sym == COMMA) {
						read();
						if (t.sym == ID) {
							rec = table->find(t.id);
							if (rec.addr == -1) {
								lexer->has_err("符号:" + t.id + "未定义");
								skip();
								return;
							}
							else {
								if (rec.kind == charvar || rec.kind == intvar) {
									read();
									code->scan(rec);
								}
								else {
									lexer->has_err("不是可以读取输入的值");
								}
							}
						}
						else {
							lexer->has_err("缺少标识符");
							skip();
							return;
						}
					}

					if (t.sym == RSMALL) {
						read();
					}
					else {
						lexer->has_err("缺少右小括号");
						skip(all_sym);
					}
					//fout << "输入语句" << endl;
					return;

				}
				else {
					lexer->has_err("不是可以读取输入的值");
				}
			}
		}
		else {
			lexer->has_err("缺少标识符");
			skip();
			return;
		}

	}
	else {
		lexer->has_err("缺少关键字");
		skip();
		return;
	}
}


/*<写语句> ::= printf ‘(’ string , <表达式> ‘)’ | printf ‘(’ string ‘)’| printf ‘(’<表达式>‘)’ */
void Parser::printfstat () {
	record expr;
	string text;

	if (t.sym == PRINTF) {
		read();
		if (t.sym == LSMALL) {
			read();
		}
		else {
			lexer->has_err("缺少左小括号");
			skip(all_sym);
		}

		//string,<表达式>   or   string
		if (t.sym == STRING) {
			text = t.id;
			read();
			//string,<表达式>
			if (t.sym == COMMA) {
				read();
				if (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER
					|| t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {
					expr = expression();
					if (t.sym == RSMALL) {
						read();
					}
					else {
						lexer->has_err("缺少右小括号");
						skip(all_sym);
					}
					code->prints(text);
					code->print(expr);
					//fout << "输出语句" << endl;
					return;
				}
				else {
					lexer->has_err("缺少表达式");
					skip();
					return;
				}
			}
			//string
			else if (t.sym == RSMALL) {
				read();
			}
			else {
				lexer->has_err("缺少右小括号");
				skip(all_sym);
			}
			code->prints(text);
			//fout << "输出语句" << endl;
			return;
		}

		//<表达式>
		else if (t.sym == ID || t.sym == PLUS || t.sym == MINUS || t.sym == INTEGER
				 || t.sym == ZERO || t.sym == LSMALL || t.sym == CHARACTER) {
			expr = expression();
			if (t.sym == RSMALL) {
				read();
			}
			else {
				lexer->has_err("缺少右小括号");
				skip(all_sym);
			}
			code->print(expr);
			//fout << "输出语句" << endl;
			return;
		}
		else {
			lexer->has_err("缺少输出的内容");
			skip();
			return;
		}

	}
	else {
		lexer->has_err("缺少关键字");
		skip();
		return;
	}
}


//程序部分
//////////////////////////////////////////////////////////////////////////////////////


/*<复合语句> ::= [<常量说明>] [<变量说明>] {<语句>} */
void Parser::compoundstats () {
	if (t.sym == CONST) {
		constdec();
	}
	if (t.sym == INT || t.sym == CHAR) {
		vardec();
	}
	stats();
}


/*<程序> ::= [<常量说明>] {<变量定义>;} {<函数定义>} void main ‘(’ ‘)’ ‘{’ <复合语句> ‘}’ */
void Parser::program () {
	if (t.sym == CONST) {
		constdec();
	}

	//变量定义
	while (t.sym == INT || t.sym == CHAR) {
		forwardread();
		forwardread();
		if (forward2.sym == LMID || forward2.sym == SEMI || forward2.sym == COMMA) {
			vardef();
			if (t.sym == SEMI) {
				read();
			}
			else {
				lexer->has_err("缺少分号");
				skip(all_sym);
			}
			//fout << "变量声明语句" << endl;
		}
		else if (forward2.sym == LSMALL || forward2.sym == LBIG) {
			break;
		}
		else {
			lexer->has_err("变量定义不完整");
			skip();
		}
	}

	//函数定义
	while (t.sym == VOID || t.sym == CHAR || t.sym == INT) {
		forwardread();
		if (t.sym == VOID && forward1.sym == MAIN) {
			break;
		}
		funcdef();
	}

	//<主函数> ::=  void main ‘(’ ‘)’ ‘{’ <复合语句> ‘}’
	newrecord();
	record main_rec;
	main_rec.name = "main";
	main_rec.kind = npvoidfunc;
	main_rec.size = 0;
	main_rec.value = 0;
	
	
	if (t.sym == VOID) {
		read();
		if (t.sym == MAIN) {
			read();
			if (t.sym == LSMALL) {
				read();
			}
			else {
				lexer->has_err("缺少左小括号");
				skip(all_sym);
			}

			if (t.sym == RSMALL) {
				read();
			}
			else {
				lexer->has_err("缺少右小括号");
				skip(all_sym);
			}

			table->new_level();
			table->add_global(main_rec);
			code->funcdec(main_rec);

			if (t.sym == LBIG) {
				read();
			}
			else {
				lexer->has_err("缺少左大括号");
				skip(all_sym);
			}
			compoundstats();
			if (t.sym == RBIG) {
			}
			else {
				lexer->has_err("缺少右大括号");
				skip(all_sym);
			}
			lexer->all_exit();
			//fout << "主函数结束" << endl;

		}
		else {
			lexer->has_err("代码不完整，缺少主函数main");
			skip(all_sym);
		}
	}
	else {
		lexer->has_err("代码不完整，缺少主函数main");
		skip(all_sym);
	}

}
