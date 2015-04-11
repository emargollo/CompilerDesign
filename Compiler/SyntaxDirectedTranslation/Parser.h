/*
 * Parser.h
 *
 *  Created on: Feb 28, 2015
 *      Author: Emmanuel
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "../LexicalAnalysis/LexicalAnalyzer.h"
#include "SemanticAction/SemanticVerification.h"
#include "SemanticAction/SemanticTranslation.h"
#include "SymbolTable.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdlib.h>

namespace rule{
typedef enum Rule_Type
{
  prog,
  classDeclx,
  classDecl,
  varFuncDeclx,
  varFuncDecl,
  typeDef,
  varFunc,
  progBody,
  funcDefx,
  funcDef,
  funcHead,
  funcBody,
  varDeclx,
  varDecl,
  statementx,
  statement,
  assignStat,
  statBlock,
  expr,
  pRel,
  relExpr,
  arithExpr,
  arithExprl,
  sign,
  term,
  terml,
  factor,
  varFuncCall,
  variable,
  idnestx,
  idnest,
  indicex,
  indice,
  arraySizex,
  arraySize,
  type,
  fParams,
  aParams,
  fParamsTailx,
  fParamsTail,
  aParamsTailx,
  aParamsTail,
  assignOp,
  relOp,
  addOp,
  multOp,

}Rule_Type;
}

class Parser
{
public:
  Parser ();
  virtual ~Parser();
  bool		parse(std::string fileName, bool output);
  bool 		match(Token_Type tk);



private:
  Token				mLookAhead;
  Token				mPeek;
  LexicalAnalyzer 		mLex;
  SemanticVerification		mSeV;
  std::stringstream		mSs;
  std::stringstream		mTs;
  SymbolTable*			mTableHead;

  SymbolTable*			mCurrentTable;
  TableEntry*			mFunctionEntry;
  TableEntry*			mCurrentEntry;

  void		createTable(std::string name);

  void		addTokenToStream();

  bool		prog();
  bool		classDeclx();
  bool		classDecl();

  bool		varFuncDeclx();
  bool		varFuncDecl();
  bool		typeDef();
  bool		varFunc();

  bool		progBody();
  bool		funcDefx();
  bool		funcDef();
  bool		funcHead();
  bool		funcBody();
  bool		varDeclx();
  bool		varDecl();
  bool		statementx();
  bool		statement();
  bool		assignStat();
  bool		statBlock();
  bool		expr();

  bool		pRel();

  bool		relExpr();
  bool		arithExpr();
  bool		arithExprl();
  bool		sign();
  bool		term();
  bool		terml();
  bool		factor();

  bool		varFuncCall();

  bool		variable();

  bool		idnestx();
  bool		idnest();

  bool		indicex();
  bool		indice();
  bool		arraySizex();
  bool		arraySize();
  bool		type();
  bool		type(std::string &type);
  bool		fParams();
  bool		aParams();
  bool		fParamsTailx();
  bool		fParamsTail();
  bool		aParamsTailx();
  bool		aParamsTail();
  bool		assignOp();
  bool		relOp();
  bool		addOp();
  bool		multOp();

  bool		first(Token_Type tt);
  bool		first(rule::Rule_Type rt);
  bool		follow(rule::Rule_Type rt);

  bool		skipErrors(rule::Rule_Type rt);
  bool		isEpsilonInFirst(rule::Rule_Type rt);

};

#endif /* PARSER_H_ */
