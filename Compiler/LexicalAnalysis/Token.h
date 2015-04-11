/*
 * Token.h
 *
 *  Created on: Jan 25, 2015
 *      Author: Emmanuel
 */

#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>
#include <sstream>

typedef enum Token_Type
{
	Id,
	Int,
	Float,
	Dot,
	Com,
	Semic,
	Div,
	Comm,
	Fcom,
	Mult,
	Plus,
	Minus,
	Assig,
	Equal,
	More,
	Mot,
	Less,
	Let,
	Diff,
	Opar,
	Cpar,
	Obra,
	Cbra,
	Okey,
	Ckey,
	Invalid,
	Eof,
	Id_And,
	Id_Not,
	Id_Or,
	Id_If,
	Id_Then,
	Id_Else,
	Id_For,
	Id_Class,
	Id_Int,
	Id_Float,
	Id_Get,
	Id_Put,
	Id_Return,
	Id_Program,
	epsilon
}Token_Type;


class Token
{
public:
	Token();
	virtual ~Token();

	void 			setTokenType(Token_Type tokenType);
	void 			setLexeme(std::string lexeme);
	void 			setPos(int pos);
	Token_Type 		getTokenType();
	std::string 		getLexeme();
	int 			getPos();

	std::string		toString();
	std::string		typeToString(Token_Type tk);

private:

	Token_Type 	mTokenType;
	std::string	mLexeme;
	int		mPos;
};

#endif /* TOKEN_H_ */
