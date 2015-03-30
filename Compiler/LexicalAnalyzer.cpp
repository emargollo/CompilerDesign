/*
 * LexicalAnalizer.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: Emmanuel
 */

#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer()
{
      mCurrentPos= 0;
}

LexicalAnalyzer::~LexicalAnalyzer()
{
	// TODO Auto-generated destructor stub
}

Token
LexicalAnalyzer::nextToken ()
{

  Token *tk	= NULL;
  int 	state	= 0;
  bool	istkpos = false;
  int	tkPos;
  char	lookup = ' ';


  while(tk == NULL)
  {
      lookup = nextChar();
      if(lookup != '\n' && lookup != '\t' && lookup != ' ' && !istkpos)
      {
	 istkpos = true;
	 tkPos = mCurrentPos;
      }
      state  = mTable.nextState(state, lookup);
      mCurrentPos++;
      if(mTable.isFinalState(state))
      {
	  if(mTable.isBackTrack(state) != No)
	  {
	      backTrack(mTable.isBackTrack(state));
	  }
	  tk = createToken(tkPos, mTable.getFinalType(state));
      }
  }
  return *tk;
}

void
LexicalAnalyzer::setFile (std::string file)
{
  mCurrentPos = 0;
  mFileName = file;
}

void
LexicalAnalyzer::backTrack (Table_Aux bt)
{
  switch(bt)
  {
    case Yes:
      mCurrentPos--;
      break;
    case BT_to_int:
      mCurrentPos--;
      break;
    case BT_to_float:
      mCurrentPos--;
      mCurrentPos--;
      char c;
      c = nextChar();
      while(c == '0')
      {
	  mCurrentPos--;
	  c = nextChar();
      }
      if(c == '.')
      {
	  mCurrentPos++;
      }
      mCurrentPos++;
      break;
    default:
      break;
  }
}

char
LexicalAnalyzer::nextChar ()
{
  char c;
  FILE * pFile;

  pFile = fopen ( mFileName.c_str() , "r" );
  fseek ( pFile , mCurrentPos , SEEK_SET );

  c = fgetc(pFile);

  fclose ( pFile );


  return c;
}

std::string
LexicalAnalyzer::getLexeme (int tkpos)
{
  char s[256];
  FILE * pFile;

  pFile = fopen ( mFileName.c_str() , "r" );
  fseek ( pFile , tkpos , SEEK_SET );

  fgets (s , mCurrentPos+1 - tkpos , pFile);

  return s;
}

Token*
LexicalAnalyzer::createToken (int tkPos, Token_Type tkt)
{
  Token* tk = new Token;
  tk->setLexeme(getLexeme(tkPos));
  tk->setPos(tkPos);
  tk->setTokenType(tkt);
  if(tkt == Id)
  {
      tk->setTokenType(checkReservedIds(tk->getLexeme()));
  }
  return tk;
}

Token_Type
LexicalAnalyzer::checkReservedIds (std::string id)
{
  Token_Type tt = Id;
  if(id == "and")
    tt = Id_And;
  else if(id == "not")
      tt = Id_Not;
  else if(id == "Or")
      tt = Id_Or;
  else if(id == "if")
      tt = Id_If;
  else if(id == "then")
        tt = Id_Then;
  else if(id == "else")
      tt = Id_Else;
  else if(id == "for")
      tt = Id_For;
  else if(id == "class")
    tt = Id_Class;
  else if(id == "int")
      tt = Id_Int;
  else if(id == "float")
      tt = Id_Float;
  else if(id == "get")
      tt = Id_Get;
  else if(id == "put")
      tt = Id_Put;
  else if(id == "return")
      tt = Id_Return;
  else if(id == "program")
        tt = Id_Program;
  return tt;
}











