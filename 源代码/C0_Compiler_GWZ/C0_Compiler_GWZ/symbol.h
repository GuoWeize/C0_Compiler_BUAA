#ifndef SYMBOL_H
#define SYMBOL_H
#include <iostream>

//�ʷ���Ԫ���ͱ�
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


//��ʶ������ (���ű���ʹ��)
enum kind {
	charconst, intconst, 
	charvar, intvar,
	voidfunc, charfunc, intfunc,	//�в�������
	npvoidfunc, npcharfunc, npintfunc,	//�޲�������
	charpara, intpara,
	chararray, intarray
};


#endif