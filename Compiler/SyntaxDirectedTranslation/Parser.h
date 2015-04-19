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
#include <algorithm>    // std::reverse
#include <fstream>
#include <sstream>
#include <math.h>
#include <string>
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

typedef struct Attributes
{
  std::string type;
  std::string nest;
  std::string id;
}Attributes;

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
  SemanticTranslation		mMoon;
  std::stringstream		mSs;
  std::stringstream		mTs;
  std::stringstream		mMoonCode;
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
  bool		typeDef(std::string& Tr);
  bool		varFunc(std::string& Tr);

  bool		progBody();
  bool		funcDefx();
  bool		funcDef();
  bool		funcHead(std::string& Hr);
  bool		funcBody(std::string& Fr);
  bool		varDeclx();
  bool		varDecl();
  bool		statementx(std::string &Sxr);
  bool		statement(std::string& Sr);
  bool		assignStat();
  bool		statBlock(std::string& Hr);
  bool		expr(std::string& Es, std::string& Eid);

  bool		pRel(std::string& Ai, std::string& Ps, std::string& Aidi, std::string& Pid);

  bool		relExpr(std::string& Rs, std::string& Rid);
  bool		arithExpr(std::string& As, std::string& Aid);
  bool		arithExprl(std::string& Ti, std::string& Als, std::string& Tidi, std::string& Alid);
  bool		sign();
  bool		term(std::string& Ts, std::string& Tids);
  bool		terml(std::string& Fi, std::string& Tls, std::string& Fidi, std::string& Tlid);
  bool		factor(std::string& Fs, std::string& Fids);

  bool		varFuncCall(std::string id, std::string nest);

  bool		variable(std::string& Vs, std::string & Vid, std::string& nest);

  bool		idnestx(std::string& nest, std::string& Ii, std::string& Ixs, std::string& n);
  bool		idnest(std::string& nest, std::string& Is, std::string& n);

  bool		indicex(std::string& Is, std::string& var, int& amount);
  bool		indicex(std::string nest, std::string& Is, std::string& var, int& amount);
  bool		indice(std::string& Is, int& amount);

  bool		arraySizex();
  bool		arraySize();
  bool		type();
  bool		type(std::string &type);
  bool		fParams();
  bool		aParams(std::string id, std::string nest);
  bool		fParamsTailx();
  bool		fParamsTail();
  bool		aParamsTailx(std::vector<std::string>& vs);
  bool		aParamsTail(std::string& Es);
  bool		assignOp();
  bool		relOp(std::string& Op);
  bool		addOp(std::string& Op);
  bool		multOp(std::string& Op);

  bool		first(Token_Type tt);
  bool		first(rule::Rule_Type rt);
  bool		follow(rule::Rule_Type rt);

  bool		skipErrors(rule::Rule_Type rt);
  bool		isEpsilonInFirst(rule::Rule_Type rt);

};

#endif /* PARSER_H_ */
