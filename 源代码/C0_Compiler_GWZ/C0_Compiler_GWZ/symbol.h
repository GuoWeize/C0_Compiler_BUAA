#ifndef SYMBOL_H
#define SYMBOL_H
#include <iostream>

//词法单元类型表
enum symbol {
	NULLSYM,
	CONST, VOID, CHAR, INT,
	MAIN, IF, ELSE, SWITCH, CASE, DEFAULT, WHILE, RETURN, SCANF, PRINTF,
	ID, INTEGER, ZERO, CHARACTER, STRING,
	PLUS,		//	+
	MINUS,		//	-
	STAR,		//	*
	DIV,		//	/
	EQ,			//	==
	BIG,		//	>
	LESS,		//	<
	BIGEQ,		//	>=
	LESSEQ,		//	<=
	NOTEQ,		//	!=
	LSMALL,		//	(
	RSMALL,		//	)
	LMID,		//	[
	RMID,		//	]
	LBIG,		//	{
	RBIG,		//	}
	COMMA,		//	,
	SEMI,		//	;
	COLON,		//	:
	ASSIGN,		//	=
};


//标识符类型 (符号表中使用)
enum kind {
	charconst, intconst, 
	charvar, intvar,
	voidfunc, charfunc, intfunc,	//有参数函数
	npvoidfunc, npcharfunc, npintfunc,	//无参数函数
	charpara, intpara,
	chararray, intarray
};


#endif