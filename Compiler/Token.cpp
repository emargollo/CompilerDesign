/*
 * Token.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: Emmanuel
 */

#include "Token.h"

Token::Token()
{
	mTokenType = Invalid;
	mPos = 0;
}

Token::~Token()
{
	// TODO Auto-generated destructor stub
}

void Token::setLexeme(std::string lexeme)
{
	mLexeme = lexeme;
}

void Token::setPos(int pos)
{
	mPos = pos;
}

void Token::setTokenType(Token_Type tokenType)
{
	mTokenType = tokenType;
}

std::string Token::getLexeme()
{
	return mLexeme;
}

int Token::getPos()
{
	return mPos;
}

Token_Type Token::getTokenType()
{
	return mTokenType;
}

std::string
Token::toString ()
{
  std::stringstream os;
  std::string s;
  os << "Type: " << typeToString(mTokenType) << ". Lexeme: \"" << mLexeme << "\" Location on file: " << mPos;
  s = os.str();
  return s;
}

std::string
Token::typeToString (Token_Type tk)
{
  std::string s;
  switch(tk)
  {
    case Id:
      s = "Id";
      break;
    case Int:
      s = "Int";
      break;
    case Float:
      s = "Float";
      break;
    case Dot:
      s = "Dot";
      break;
    case Com:
      s = "Com";
      break;
    case Semic:
      s = "Semic";
      break;
    case Div:
      s = "Div";
      break;
    case Comm:
      s = "Comm";
      break;
    case Fcom:
      s = "Fcom";
      break;
    case Mult:
      s = "Mult";
      break;
    case Plus:
      s = "Plus";
      break;
    case Minus:
      s = "Minus";
      break;
    case Assig:
      s = "Assig";
      break;
    case Equal:
      s = "Equal";
      break;
    case More:
      s = "More";
      break;
    case Mot:
      s = "Mot";
      break;
    case Less:
      s = "Less";
      break;
    case Let:
      s = "Let";
      break;
    case Diff:
      s = "Diff";
      break;
    case Opar:
      s = "Opar";
      break;
    case Cpar:
      s = "Cpar";
      break;
    case Obra:
      s = "Obra";
      break;
    case Cbra:
      s = "Sbra";
      break;
    case Okey:
      s = "Okey";
      break;
    case Ckey:
      s = "Ckey";
      break;
    case Invalid:
      s = "Invalid";
      break;
    case Eof:
      s = "Eof";
      break;
    case Id_And:
      s = "Id_And";
      break;
    case Id_Not:
      s = "Id_Not";
      break;
    case Id_Or:
      s = "Id_Or";
      break;
    case Id_If:
      s = "Id_If";
      break;
    case Id_Then:
      s = "Id_Then";
      break;
    case Id_Else:
      s = "Id_Else";
      break;
    case Id_For:
      s = "Id_For";
      break;
    case Id_Class:
      s = "Id_Class";
      break;
    case Id_Int:
      s = "Id_Int";
      break;
    case Id_Float:
      s = "Id_Float";
      break;
    case Id_Get:
      s = "Id_Get";
      break;
    case Id_Put:
      s = "Id_Put";
      break;
    case Id_Return:
      s = "Id_Return";
      break;
    case Id_Program:
	s = "Id_Program";
	break;
    case epsilon:
	s = "epsilon";
	break;
  }
  return s;
}
