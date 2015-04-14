/*
 * Parser.cpp
 *
 *  Created on: Feb 28, 2015
 *      Author: Emmanuel
 */

#include "Parser.h"

Parser::Parser ()
{
  // TODO Auto-generated constructor stub
}

Parser::~Parser ()
{
  // TODO Auto-generated destructor stub
}

void
Parser::addTokenToStream()
{
  mTs << "Token: [ " << mLookAhead.toString() << " ]\n";
}

bool
Parser::parse (std::string fileName, bool output)
{
  bool	success;
  mLex.setFile(fileName);
  mPeek = mLex.nextToken();
  mLookAhead = mPeek;
  addTokenToStream();
  mPeek = mLex.nextToken();
  mCurrentTable = NULL;
  if(prog() && match(Eof))
  {
      success = true;
  }
  else{
    success = false;
  }
  //std::cout<<mSs.str()<<std::endl;
  if(output){
    std::ofstream myfile;
    myfile.open("LexicalOutput");
    myfile << mTs.str();
    myfile.close();
    myfile.open ("SyntaxOutput.txt");
    myfile << mSs.str();
    myfile.close();
    myfile.open("SymbolTables.txt");
    myfile<<mTableHead->print();
    myfile.close();
    myfile.open("MoonCode.txt");
    myfile<<mMoon.getCode();
    myfile.close();
  }
  //std::cout<<mTableHead->print();
  std::cout<<mMoon.getCode();
  return success;
}

bool
Parser::match (Token_Type tk)
{
  if(mLookAhead.getTokenType() == tk)
  {
      mSs<<mLookAhead.getLexeme()<<std::endl;
      mLookAhead = mPeek;
      addTokenToStream();
      mPeek = mLex.nextToken();
      while(mLookAhead.getTokenType() == Comm || mLookAhead.getTokenType() == Fcom)
      {
	  mSs<<mLookAhead.getLexeme()<<std::endl;
	  mLookAhead = mPeek;
	  addTokenToStream();
	  mPeek = mLex.nextToken();
      }
      return true;
  }

  mSs<<"Syntax error at " <<mLookAhead.getPos()<<" Expected "<< mLookAhead.typeToString(tk)
      <<" Recieved "<< mLookAhead.typeToString(mLookAhead.getTokenType())  <<" Instead."<<std::endl;
  mLookAhead = mPeek;
  mPeek = mLex.nextToken();

  while(mLookAhead.getTokenType() == Comm || mLookAhead.getTokenType() == Fcom)
  {
	  mSs<<mLookAhead.getLexeme()<<std::endl;
	  mLookAhead = mPeek;
	  addTokenToStream();
	  mPeek = mLex.nextToken();
  }
  return false;
}

void
Parser::createTable(std::string name)
{
  SymbolTable *t = new SymbolTable(name, mCurrentTable);
  mCurrentTable = t;
}

bool
Parser::prog ()
{
  bool success = skipErrors(rule::prog);
  if(first(rule::classDeclx))
  {
      createTable("Global");
      mTableHead = mCurrentTable;
      if(classDeclx() && progBody())
      {
	  mSs<<"<prog> -> <classDecl*><progBody>"<<std::endl;
      }
      else
	success = false;
  }
  else
    success = false;
  return(success);
}

bool
Parser::classDeclx ()
{
  bool success = skipErrors(rule::classDeclx);
  if(first(rule::classDecl))
  {
      if(classDecl() && classDeclx())
      {
	  mSs<<"<classDecl*> -> <classDecl><classDecl*>"<<std::endl;
      }
      else
	success = false;
  }
  else if(follow(rule::classDeclx))
  {
    mSs<<"<classDecl*> -> epsilon"<<std::endl;
  }
  else
    success = false;
  return success;
}

bool
Parser::classDecl ()
{
  bool success = skipErrors(rule::classDecl);
  if(first(Id_Class))
  {
      mCurrentTable->insert("class", mCurrentEntry);		//Enter class entry in table
      createTable("class");	//Create and enter new Table

      mCurrentEntry->setLink(mCurrentTable);	//Links Entry to new Table
      mCurrentEntry->setType(mLookAhead.getLexeme());
      mCurrentEntry->setStructure(structure::Class);
      mCurrentEntry->setKind(kind::Class);

      if(match(Id_Class)){
	  mCurrentEntry->setName(mSeV.checkDoubleDeclaration(mLookAhead.getLexeme(), mCurrentTable->getPreviousTable(), success));
	  mCurrentTable->setName(mLookAhead.getLexeme());	//Symbol Table Functions
	  if(match(Id) && match(Okey) && varFuncDeclx() && match(Ckey) && match(Semic))
	  {
	      mSs<< "<classDecl> -> class id {<varDecl*><funcDef*>};"<<std::endl;
	      mCurrentTable = mCurrentTable->getPreviousTable();	//Exit Class Symbol Table
	  }
	  else{success = false;}
      }
      else{success = false;}
  }
  else{success = false;}
  return success;
}

bool
Parser::varFuncDeclx ()
{
  bool success = skipErrors(rule::varFuncDeclx);
  if(first(rule::varFuncDecl))
    {
        if(varFuncDecl() && varFuncDeclx())
        {
  	  mSs<<"<varFuncDeclx*> -> <varFuncDeclx><varFuncDeclx*>"<<std::endl;

        }
        else{success = false;}
    }
    else if(follow(rule::varFuncDeclx))
    {
      mSs<<"<varFuncDeclx*> -> epsilon"<<std::endl;

    }
    else{success = false;}
  return(success);
}

bool
Parser::varFuncDecl ()
{
  bool success = skipErrors(rule::varFuncDecl);
  if(first(rule::typeDef))
  {
      std::string Hr;
      if(typeDef(Hr) && varFunc(Hr))
      {
	  mSs<< "<varFuncDecl> -> <typeDef><varFunc>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::typeDef (std::string& Tr)
{
  bool success = skipErrors(rule::typeDef);

  if(first(rule::type))
  {
      std::string typeDefTC;
      mCurrentTable->insert("variable/function", mCurrentEntry);		//Enter class entry in table
      //mCurrentEntry->setType(mLookAhead.getLexeme());

      if(type(typeDefTC)){
	  mCurrentEntry->setName(mSeV.checkDoubleDeclaration(mLookAhead.getLexeme(), mCurrentTable, success));
	  if(match(Id))
	  {
	      Tr = typeDefTC;
	      mCurrentEntry->setType(typeDefTC);
	      mSs<< "<typeDef> -> <type>id"<<std::endl;

	  }
	else{success = false;}
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::varFunc (std::string& Tr)
{
  bool success = skipErrors(rule::varFunc);
  if(first(rule::arraySizex) || first(Semic))
  {
      mCurrentEntry->setKind(kind::Variable);
      mCurrentEntry->setStructure(structure::Simple);
      if(arraySizex() && match(Semic))
      {
	mMoon.generateVariableDeclaration(mCurrentEntry, mCurrentTable);
	mSs<< "<varFunc> -> <arraySize*>;"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Opar))
  {
      createTable(std::string(mCurrentTable->getName() + "_" + mCurrentEntry->getName()));

      mCurrentEntry->setKind(kind::Function);
      mCurrentEntry->setStructure(structure::Simple);
      mCurrentEntry->setLink(mCurrentTable);

      mFunctionEntry = mCurrentEntry;
      if(match(Opar) && fParams() && match(Cpar) && funcBody(Tr) && match(Semic))
      {
	mSs<< "<varFunc> -> ( <fParams> ) <funcBody>;"<<std::endl;
      }

      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::progBody ()
{
  bool success = skipErrors(rule::progBody);
  if(first(rule::funcDefx))
  {

      if(funcDefx()){
	  std::string Fr = "int";
	  mCurrentTable->insert("Program", mCurrentEntry);
	  createTable("Program");
	  mCurrentEntry->setKind(kind::Function);
	  mCurrentEntry->setStructure(structure::Simple);
	  mCurrentEntry->setType(mLookAhead.getLexeme());
	  mCurrentEntry->setLink(mCurrentTable);
	  mMoon.generateEntryDirective();
	  if(match(Id_Program) && funcBody(Fr) && match(Semic) )
	  {
	      mSs<< "<progBody> -> program<funcBody>;<funcDef*>" << std::endl;

	  }
	  else{success = false;}
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::funcDefx ()
{
  bool success = skipErrors(rule::funcDefx);
  if(first(rule::funcDef))
  {
      if(funcDef() && funcDefx())
      {
	mSs<<"<funcDef*> -> <funcDef><funcDef*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::funcDefx))
  {
      mSs<<"<funcDef*> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::funcDef ()
{
  std::string Hr;
  bool success = skipErrors(rule::funcDef);
  if(first(rule::funcHead))
  {
      if(funcHead(Hr) && funcBody(Hr) && match(Semic))
      {
	  mSs<< "<funcDef> -> <funcHead><funcBody>;"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::funcHead (std::string& Hr)
{
  bool success = skipErrors(rule::funcHead);
  if(first(rule::type))
  {
      std::string funcHeadTC;

      mCurrentTable->insert("func", mCurrentEntry);

      createTable("Function");

      mCurrentEntry->setStructure(structure::Simple);
      mCurrentEntry->setKind(kind::Function);
      //mCurrentEntry->setType(mLookAhead.getLexeme());
      mCurrentEntry->setLink(mCurrentTable);
      if(type(funcHeadTC)){
	  mCurrentEntry->setType(funcHeadTC);
	  mCurrentTable->setName(mLookAhead.getLexeme());
	  mCurrentEntry->setName(mSeV.checkDoubleDeclaration(mLookAhead.getLexeme(), mCurrentTable->getPreviousTable(), success));
	  mFunctionEntry = mCurrentEntry;
	  if(match(Id) && match(Opar) && fParams() && match(Cpar))
	  {
	    mSs<< "<funcHead>	-> <type>id(<fParams>)"<<std::endl;
	    Hr = funcHeadTC;
	  }
	  else{success = false;}
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::funcBody (std::string& Fr)
{
  bool success = skipErrors(rule::funcBody);
  if(first(Okey))
  {
      if(match(Okey) && varDeclx() && statementx(Fr) && match(Ckey))
      {
	  mCurrentTable = mCurrentTable->getPreviousTable();//Exit Method/Program/Function Tables
	  mSs<< "<funcBody> -> {<varDecl*><statement*>}"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::varDeclx ()
{
  bool success = skipErrors(rule::varDeclx);
  if(first(rule::varDecl) && mPeek.getTokenType() == Id)
    {
        if(varDecl() && varDeclx())
        {
	  mSs<<"<varDecl*> -> <varDecl><varDecl*> "<<std::endl;

        }
        else{success = false;}
    }
    else if(follow(rule::varDeclx))
    {
      mSs<<"<varDecl*> -> epsilon"<<std::endl;

    }
    else{success = false;}
  return(success);
}

bool
Parser::varDecl ()
{
  bool success = skipErrors(rule::varDecl);
  if(first(rule::type))
  {
      std::string varDeclTC, varID;
      mCurrentTable->insert("variable", mCurrentEntry);
      mCurrentEntry->setKind(kind::Variable);
      mCurrentEntry->setStructure(structure::Simple);
      if(type(varDeclTC) ){
	  mCurrentEntry->setType(varDeclTC);
	  varID = mSeV.checkDoubleDeclaration(mLookAhead.getLexeme(), mCurrentTable, success);
	  mCurrentEntry->setName(varID);
	  if(match(Id) && arraySizex() && match(Semic))
	  {
	      mMoon.generateVariableDeclaration(mCurrentEntry, mCurrentTable);
	      mSs<< "<varDecl> -> <type>id<arraySize*>;"<<std::endl;

	  }
	  else{success = false;}
     }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::statementx (std::string& Sxr)
{
  bool success = skipErrors(rule::statementx);
  if(first(rule::statement))
  {
      if(statement(Sxr) && statementx(Sxr))
      {
      mSs<<"<statement*> -> <statement><statement*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::statementx))
  {
  mSs<<"<statement*> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::statement (std::string& Sr)
{
  std::string Es, Rs, Vs, Rid, Eid;
  bool success = skipErrors(rule::statement);
  if(first(rule::assignStat))
  {
      if(assignStat() && match(Semic))
      {
	  mSs<<"<statement> -> <assignStat>;"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_If))
  {
      if(match(Id_If) && match(Opar) && expr(Es, Eid) && match(Cpar) && match(Id_Then)
	  && statBlock(Sr) && match(Id_Else) && statBlock(Sr) && match(Semic))
      {
	  mSs<< "<statement> -> if(<expr>)then<statBlock>else<statBlock>;"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_For))
  {
      std::string forVar;
      bool newVar = true;
      if(match(Id_For) && match(Opar) && type(forVar))
      {
	  newVar = mSeV.checkDoubleDeclaration(mLookAhead.getLexeme(), mCurrentTable);
	  if(newVar)
	  {
	      mCurrentTable->insert("forVar", mCurrentEntry);
	      mCurrentEntry->setName(mLookAhead.getLexeme());
	      mCurrentEntry->setKind(kind::Variable);
	      mCurrentEntry->setStructure(structure::Simple);
	  }
	  if(match(Id) && assignOp() &&
	  expr(Es, Eid) && match(Semic) && relExpr(Rs, Rid) && match(Semic) && assignStat()
	  && match(Cpar) && statBlock(Sr) && match(Semic))
	  {
	      mSs<< "<statement> -> for(<type>id<assignOp><expr>;<relExpr>;<assignStat>)<statBlock>;"<<std::endl;
	      if(newVar)
		mCurrentEntry->setType(forVar);
	  }
	  else{success = false;}
      }
      else{success = false;}
  }
  else if(first(Id_Get))
  {
      if(match(Id_Get) && match(Opar) && variable(Vs) && match(Cpar) && match(Semic))
      {
	  mSs<< "<statement> -> get(<variable>);"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Put))
  {
      if(match(Id_Put) && match(Opar) && expr(Es, Eid) && match(Cpar) && match(Semic))
      {
	  mSs<< "<statement> -> put(<expr>);"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Return))
  {
      if(match(Id_Return) && match(Opar) && expr(Es, Eid) && match(Cpar) && match(Semic))
      {
	  mSeV.checkReturnType(Sr, Es, success);
	  mSs<< "<statement> -> return(<expr>);"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::assignStat ()
{
  std::string Es, Vs, Eid;
  bool success = skipErrors(rule::assignStat);
  if(first(rule::variable))
  {
      if(variable(Vs) && assignOp() && expr(Es, Eid))
      {
	  mSs<< "<assignStat> -> <variable><assignOp><expr>"<<std::endl;
	  mSeV.checkAssigTypes(Vs, Es, success);
	  //TODO: Add AssigOP.
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::statBlock (std::string& Hr)
{
  bool success = skipErrors(rule::statBlock);
  if(first(Okey))
  {
      if(match(Okey) && statementx(Hr) && match(Ckey))
      {
	  mSs<< "<statBlock> -> {<statement*>} "<<std::endl;

      }

  }
  else if(first(rule::statement))
  {
      if(statement(Hr))
      {
	  mSs<< "<statBlock> -> <statement> "<<std::endl;

      }
  }
  else if(follow(rule::statBlock))
  {
      mSs<<  "<statBlock> -> epsilon "<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::expr (std::string& Es, std::string Eid)
{
  std::string As, Ps, Aid, Pid;
  bool success = skipErrors(rule::expr);
  if(first(rule::arithExpr))
  {
      if(arithExpr(As, Aid) && pRel(As, Ps, Aid, Pid))
      {
	  mSs<< "<expr> -> <arithExpr><pRel>" <<std::endl;
	  Es = Ps;
	  Eid = Pid;
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::pRel (std::string& Ai, std::string& Ps, std::string& Aidi, std::string& Pid)
{
  std::string As, Aid, Op;
  bool success = skipErrors(rule::pRel);
  if(first(rule::relOp))
  {
      if(relOp(Op) && arithExpr(As, Aid))
      {
	  mSs<< "<pRel> -> <relOp><arithExpr>" <<std::endl;
	  Ps = mSeV.checkOperatorTypes(Ai, As, success);
	  //TODO: Add RelOpGenerator Pid
      }
      else{success = false;}
  }
  else if(follow(rule::pRel))
  {
      mSs<< "<pRel> -> epsilon" <<std::endl;
      Ps = Ai;
      Pid = Aidi;
  }
  else{success = false;}
  return(success);
}

bool
Parser::relExpr (std::string& Rs, std::string& Rid)
{
  std::string A1s, A2s, Aid1, Aid2, Op;
  bool success = skipErrors(rule::relExpr);
  if(first(rule::arithExpr))
  {
      if(arithExpr(A1s, Aid1) && relOp(Op) && arithExpr(A2s, Aid2))
      {
	  mSs<< "<relExpr> -> <arithExpr><relOp><arithExpr>" <<std::endl;
	  mSeV.checkOperatorTypes(A1s, A2s, success);
	  //TODO: Add RelOpGenerator
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::arithExpr (std::string& As, std::string& Aid)
{
  std::string Ts, Als, Tid, Alid;
  bool success = skipErrors(rule::arithExpr);
  if(first(rule::term))
  {
      if(term(Ts, Tid) && arithExprl(Ts, Als, Tid, Alid))
      {
	  mSs<< "<arithExpr> -> <term><arithExpr'>"<<std::endl;
	  As = Als;
	  Aid = Alid;
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::arithExprl (std::string& Ti, std::string& Als, std::string& Tidi, std::string& Alid)
{
  std::string Ts, Al2s, Tid, Alid2, Op;
  bool success = skipErrors(rule::arithExprl);
  if(first(rule::addOp))
  {
      if(addOp(Op) && term(Ts, Tid) && arithExprl(Ts, Al2s, Tid, Alid2))
      {
	  mSs<< "<arithExpr'> -> <addOp><term><arithExpr'>" <<std::endl;
	  Als = mSeV.checkOperatorTypes(Ti, Al2s, success);
	  //TODO: Add ARITHGen Alid
	  Alid = mMoon.generateArithOp(Tidi, Alid2, mCurrentEntry, mCurrentTable, Op);
      }
      else{success = false;}
  }
  else if(follow(rule::arithExprl))
  {
      mSs<< "<arithExpr'> -> epsilon" <<std::endl;
      Als = Ti;
      Alid = Tidi;
  }
  else{success = false;}
  return(success);
}

bool
Parser::sign ()
{
  bool success = skipErrors(rule::sign);
  if(first(Plus))
  {
      if(match(Plus))
      {
	  mSs<< "<sign> -> +"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Minus))
  {
      if(match(Minus))
      {
	  mSs<< "<sign> -> -"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::term (std::string& Ts, std::string& Tids)
{
  std::string Fs, Tls, Fid, Tlid;
  bool success = skipErrors(rule::term);
  if(first(rule::factor))
  {
      if(factor(Fs, Fid) && terml(Fs, Tls, Fid, Tlid))
      {
	  mSs<< "<term> -> <factor><term'>"<<std::endl;
	  Ts = Tls;
	  Tids = Tlid;
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::terml (std::string& Fi, std::string& Tls, std::string& Fidi, std::string& Tlid)
{
  std::string Fs, Tl2s, Fids, Tlid2, Op;
  bool success = skipErrors(rule::terml);
  if(first(rule::multOp))
  {
      if(multOp(Op) && factor(Fs, Fids) && terml(Fs, Tl2s, Fids, Tlid2))
      {
	  mSs<< "<term'> -> <multOp><factor><term'>" <<std::endl;
	  Tls = mSeV.checkOperatorTypes(Fi, Tl2s, success);
	  //TODO: CALL ArithGenerator. Tlid
	  std::cout<<Fidi<<std::endl;
	  Tlid = mMoon.generateArithOp(Fidi, Tlid2, mCurrentEntry, mCurrentTable, Op);
      }
      else{success = false;}
  }
  else if(follow(rule::terml))
  {
      mSs<< "<term'> -> epsilon" <<std::endl;
      Tls = Fi;
      Tlid = Fidi;
  }
  else{success = false;}
  return(success);
}

bool
Parser::factor (std::string& Fs, std::string& Fids)
{
  std::string Es, F2s, Is, Fid2s, Eid;
  std::string nest = "null";
  int amount = 0;
  bool success = skipErrors(rule::factor);
  if(first(Id))
  {
      std::string id = mLookAhead.getLexeme();
      if(match(Id) && indicex(Is, id, amount) &&  idnestx(id, Is, Is, nest) && varFuncCall(id, nest))
      {
	  mSs<< "<factor>	-> id<idnest*><varFuncCall>"<<std::endl;
	  Fs = Is;
	  Fids = id;
      }
      else{success = false;}
  }
  else if(first(Float))
  {
      if(match(Float))
      {
	  mSs<< "<factor>	-> num"<<std::endl;
	  Fs = "float";
      }
      else{success = false;}
  }
  else if(first(Int))
  {
      if(match(Int))
      {
	mSs<< "<factor> -> num"<<std::endl;
	Fs = "int";
      }
      else{success = false;}
  }
  else if(first(Opar))
  {
      if(match(Opar) && arithExpr(Es, Eid) && match(Cpar)	)
      {
	mSs<< "<factor> -> (<arithExpr>)"<<std::endl;
	Fs = Es;
	Fids = Eid;
      }
      else{success = false;}
  }
  else if(first(Id_Not))
  {
      if(match(Id_Not) && factor(F2s, Fid2s))
      {
	mSs<< "<factor> -> not<factor>"<<std::endl;
	Fs = F2s;
      }
      else{success = false;}
  }
  else if(rule::sign)
  {
      if(sign() && factor(F2s, Fid2s))
      {
      	mSs<< "<factor> -> <sign><factor>"<<std::endl;
      	Fs = F2s;
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::varFuncCall(std::string id, std::string nest)
{
  bool success = skipErrors(rule::varFuncCall);
  if(first(Opar))
  {
      if(match(Opar) && aParams(id, nest) && match(Cpar))
      {
	mSs<< "<varFuncCall> -> (<aParams>)"<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::varFuncCall))
  {
      mSs<< "<varFuncCall> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::variable (std::string& Vs)
{
  bool success = skipErrors(rule::variable);
  std::string Is, id, n;
  int amount = 0;
  if(first(Id))
  {
      id = mLookAhead.getLexeme();
      if(match(Id) && indicex(Is, id, amount) && idnestx(id, Is, Is, n))
      {
	  mSs<< "<variable> -> id<indice*><idnest*>"<<std::endl;
	  Vs = Is;
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::idnestx (std::string &nest,std::string& Ii, std::string& Ixs, std::string& n)
{
  std::string Is, Ix2s;
  bool success = skipErrors(rule::idnestx);
  if(first(rule::idnest))
  {
      if(idnest(nest, Is, n) && idnestx(nest, Is, Ix2s, n))
      {
      mSs<<"<idnest*> -> <idnest><idnest*> "<<std::endl;
      Ixs = Ix2s;
      }
      else{success = false;}
  }
  else if(follow(rule::idnestx))
  {
      mSs<<"<idnest*> -> epsilon"<<std::endl;
      Ixs = Ii;
  }
  else{success = false;}
  return(success);
}

bool
Parser::idnest (std::string& nest, std::string& Is, std::string& n)
{
  std::string name;
  int amount = 0;
  bool success = skipErrors(rule::idnest);
  if(first(Dot))
  {
      if(match(Dot) ){
	  name = mLookAhead.getLexeme();
	  if( match(Id) && indicex(nest, Is, name, amount))
	  {
	    Is = mSeV.checkVarInsideNest(nest, name, mCurrentTable, success);
	    n = nest;
	    nest = name;
	    mSs<< "<idnest> -> id<indice*>."<<std::endl;

	  }
	  else{success = false;}
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::indicex (std::string& Is, std::string& var, int& amount)
{
  bool success = skipErrors(rule::indicex);
  if(first(rule::indice))
  {
      if(indice(Is, amount) && indicex(Is, var, amount))
      {
      mSs<<"<indice*> -> <indice><indice*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::indicex))
  {
      mSs<<"<indice*> -> epsilon"<<std::endl;
      Is = mSeV.checkAmountOfIndices(var, amount, mCurrentTable, success);
  }
  else{success = false;}
  return(success);
}

bool
Parser::indicex (std::string nest, std::string& Is, std::string& var, int& amount)
{
  bool success = skipErrors(rule::indicex);
  if(first(rule::indice))
  {
      if(indice(Is, amount) && indicex(nest, Is, var, amount))
      {
      mSs<<"<indice*> -> <indice><indice*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::indicex))
  {
      mSs<<"<indice*> -> epsilon"<<std::endl;
      Is = mSeV.checkAmountOfIndicesInNest(nest, var, amount, mCurrentTable, success);
  }
  else{success = false;}
  return(success);
}

bool
Parser::indice (std::string& Is, int& amount)
{
  std::string As, Aid;
  bool success = skipErrors(rule::indice);
  if(first(Obra))
  {
      if(match(Obra) && arithExpr(As, Aid) && match(Cbra))
      {
	  mSs<< "<indice>	-> [<arithmExpr>]"<<std::endl;
	  mSeV.checkOperatorTypes(As, "int", success);
	  amount++;
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::arraySizex ()
{
  bool success = skipErrors(rule::arraySizex);
  if(first(rule::arraySize))
  {
      if(arraySize() && arraySizex())
      {
	mSs<<"<arraySize*> -> <arraySize><arraySize*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::arraySizex))
  {
      mSs<<"<arraySize*> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::arraySize ()
{
  bool success = skipErrors(rule::arraySize);
  if(first(Obra))
  {
      mCurrentEntry->setStructure(structure::Array);
      if(match(Obra) ){
	  mCurrentEntry->addDimension(atoi(mLookAhead.getLexeme().c_str()));
	  if( match(Int) && match(Cbra))
	  {
	      mSs<< "<arraySize> -> [ num(int) ]"<<std::endl;

	  }
	  else{success = false;}
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::type ()
{
  bool success = skipErrors(rule::type);
  if(first(Id_Int))
  {
      if(match(Id_Int))
      {
	  mSs<< "<type> -> int"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Float))
  {
      if(match(Id_Float))
      {
	mSs<< "<type> -> float"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id))
  {
      if(match(Id))
      {
	mSs<< "<type> -> id"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::type (std::string &type)
{
  bool success = skipErrors(rule::type);
  if(first(Id_Int))
  {
      type = mLookAhead.getLexeme();
      if(match(Id_Int))
      {

	  mSs<< "<type> -> int"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Float))
  {
      type = mLookAhead.getLexeme();
      if(match(Id_Float))
      {
	mSs<< "<type> -> float"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id))
  {
      type = mSeV.checkUserType(mLookAhead.getLexeme(), mCurrentTable, success);
      if(match(Id))
      {
	mSs<< "<type> -> id"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::fParams ()
{
  std::string fParamsTC;
  bool success = skipErrors(rule::fParams);
  if(first(rule::type))
  {
      mCurrentTable->insert("param", mCurrentEntry);
      mFunctionEntry->addParameter(mLookAhead.getLexeme());
      mCurrentEntry->setKind(kind::Parameter);
      mCurrentEntry->setStructure(structure::Simple);
      if(type(fParamsTC) ){
	mCurrentEntry->setType(fParamsTC);
	mCurrentEntry->setName(mSeV.checkDoubleDeclaration(mLookAhead.getLexeme(), mCurrentTable, success));
	if(match(Id) && arraySizex() && fParamsTailx())
	{
	    mSs<< "<fParams> -> <type>id<arraySize*><fParamsTail*>"<<std::endl;

	}
	else{success = false;}
      }
      else{success = false;}
  }
  else if(follow(rule::fParams))
  {
      mSs<< "<fParams> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::aParams (std::string id, std::string nest)
{
  std::string Es, Eid;
  std::vector<std::string> VSp;
  bool success = skipErrors(rule::aParams);
  if(first(rule::expr))
  {

      if(expr(Es, Eid) && aParamsTailx())
      {
	  mSs<< "<aParams> -> <expr><aParamsTail*>"<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::aParams))
  {
      mSs<< "<aParams> -> epsilon"<<std::endl;
      mSeV.checkParameters(id, nest, VSp, mCurrentTable, success);
  }
  else{success = false;}
  return(success);
}

bool
Parser::fParamsTailx ()
{
  bool success = skipErrors(rule::fParamsTailx);
  if(first(rule::fParamsTail))
  {
      if(fParamsTail() && fParamsTailx())
      {
      mSs<<"<fParamsTail*> -> <fParamsTail><fParamsTail*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::fParamsTailx))
  {
  mSs<<"<fParamsTail*> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::fParamsTail ()
{
  std::string fParamsTailTC;
  bool success = skipErrors(rule::fParamsTail);
  if(first(Com))
  {
      if(match(Com) )
      {
	  mCurrentTable->insert("param", mCurrentEntry);
	  mFunctionEntry->addParameter(mLookAhead.getLexeme());

	  mCurrentEntry->setKind(kind::Parameter);
	  mCurrentEntry->setStructure(structure::Simple);
	  if( type(fParamsTailTC) )
	  {
	    mCurrentEntry->setType(fParamsTailTC);
	    mCurrentEntry->setName(mSeV.checkDoubleDeclaration(mLookAhead.getLexeme(), mCurrentTable, success));
	    if( match(Id) && arraySizex())
	    {
		mSs<< "<fParamsTail> -> ,<type>id<arraySize*>" <<std::endl;

	    }
	    else{success = false;}
	  }
      }
  }
  else{success = false;}
  return(success);
}

bool
Parser::aParamsTailx ()
{
  bool success = skipErrors(rule::aParamsTailx);
  if(first(rule::aParamsTail))
  {
      if(aParamsTail() && aParamsTailx())
      {
      mSs<<"<fParamsTail*> -> <fParamsTail><fParamsTail*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::fParamsTailx))
  {
  mSs<<"<fParamsTail*> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::aParamsTail ()
{
  std::string Es, Eid;
  bool success = skipErrors(rule::aParamsTail);
  if(first(Com))
  {
      if(match(Com) && expr(Es, Eid))
      {
	  mSs<< "<aParamsTail> -> ,<expr>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::assignOp ()
{
  bool success = skipErrors(rule::assignOp);
  if(first(Assig))
  {
      if(match(Assig))
      {
	mSs<< "<assignOp> -> ="<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::relOp (std::string& Op)
{
  Op = mLookAhead.getLexeme();
  bool success = skipErrors(rule::relOp);
  if(first(Equal))
  {
      if(match(Equal))
      {
	  mSs<< "<relOp> -> =="<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Diff))
  {
      if(match(Diff))
      {
	mSs<< "<relOp> -> <>"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Less))
  {
      if(match(Less))
      {
      mSs<< "<relOp> -> <"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(More))
  {
      if(match(More))
      {
      mSs<< "<relOp> -> >"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Let))
  {
      if(match(Let))
      {
      mSs<< "<relOp> -> <="<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Mot))
  {
      if(match(Mot))
      {
      mSs<< "<relOp> -> >="<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::addOp (std::string& Op)
{
  Op = mLookAhead.getLexeme();
  bool success = skipErrors(rule::addOp);
  if(first(Plus))
  {
      if(match(Plus))
      {
	  mSs<< "<addOp> -> +"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Minus))
  {
      if(match(Minus))
      {
	mSs<< "<addOp> -> -"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Or))
  {
      if(match(Id_Or))
      {
	mSs<< "<addOp> -> or"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::multOp (std::string& Op)
{
  Op = mLookAhead.getLexeme();
  bool success = skipErrors(rule::multOp);
  if(first(Mult))
  {
      if(match(Mult))
      {
	  mSs<< "<multOp> -> *"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Div))
  {
      if(match(Div))
      {
	mSs<< "<multOp> -> /"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_And))
  {
      if(match(Id_And))
      {
	mSs<< "<multOp> -> and"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::first (Token_Type tt)
{
  if(mLookAhead.getTokenType() == tt)
    return true;
  return false;
}

bool
Parser::first (rule::Rule_Type rt)
{
  switch(rt)
  {
      case rule::prog:
	if(mLookAhead.getTokenType() == Id_Class
	    || mLookAhead.getTokenType() == Id_Program
	    || mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::classDeclx:
	if(mLookAhead.getTokenType() == Id_Class
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Id_Program
	    || mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::classDecl:
	if(mLookAhead.getTokenType() == Id_Class)
	  return true;
	break;
      case rule::varFuncDeclx:
      	if(mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
      	  return true;
      	break;
      case rule::varFuncDecl:
      	if(mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
      	  return true;
      	break;
      case rule::typeDef:
      	if(mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
      	  return true;
      	break;
      case rule::varFunc:
      	if(mLookAhead.getTokenType() == Obra
	    || mLookAhead.getTokenType() == Semic
	    || mLookAhead.getTokenType() == Opar)
      	  return true;
      	break;
      case rule::progBody:
	if(mLookAhead.getTokenType() == Id_Program
	    ||mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::funcDefx:
	if(mLookAhead.getTokenType() == Id_Program
	    ||mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::funcDef:
	if(mLookAhead.getTokenType() == Id_Int
	  || mLookAhead.getTokenType() == Id_Float
	  || mLookAhead.getTokenType() == Id)
	return true;
      break;
      case rule::funcHead:
	if(mLookAhead.getTokenType() == Id_Int
	  || mLookAhead.getTokenType() == Id_Float
	  || mLookAhead.getTokenType() == Id)
	return true;
      break;
      case rule::funcBody:
	if(mLookAhead.getTokenType() == Okey)
	  return true;
	break;
      case rule::varDeclx:
	if(mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == epsilon
	  || mLookAhead.getTokenType() == Id_Float
	  || mLookAhead.getTokenType() == Id)
	return true;
      break;
      case rule::varDecl:
	if(mLookAhead.getTokenType() == Id_Int
	  || mLookAhead.getTokenType() == Id_Float
	  || mLookAhead.getTokenType() == Id)
	return true;
      break;
      case rule::statementx:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Id_If
	    || mLookAhead.getTokenType() == Id_For
	    || mLookAhead.getTokenType() == Id_Get
	    || mLookAhead.getTokenType() == Id_Put
	    || mLookAhead.getTokenType() == Id_Return)
	  return true;
	break;
      case rule::statement:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == Id_If
	    || mLookAhead.getTokenType() == Id_For
	    || mLookAhead.getTokenType() == Id_Get
	    || mLookAhead.getTokenType() == Id_Put
	    || mLookAhead.getTokenType() == Id_Return)
	  return true;
	break;
      case rule::assignStat:
	if(mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::statBlock:
	if(mLookAhead.getTokenType() == Okey
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == Id_If
	    || mLookAhead.getTokenType() == Id_For
	    || mLookAhead.getTokenType() == Id_Get
	    || mLookAhead.getTokenType() == Id_Put
	    || mLookAhead.getTokenType() == Id_Return)
	  return true;
	break;
      case rule::expr:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == Int
	    || mLookAhead.getTokenType() == Float
	    || mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == Id_Not
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::pRel:
      	if(mLookAhead.getTokenType() == Equal
	    || mLookAhead.getTokenType() == epsilon
      	    || mLookAhead.getTokenType() == Diff
      	    || mLookAhead.getTokenType() == More
      	    || mLookAhead.getTokenType() == Less
      	    || mLookAhead.getTokenType() == Mot
      	    || mLookAhead.getTokenType() == Let)
      	  return true;
      	break;
      case rule::relExpr:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == Int
	    || mLookAhead.getTokenType() == Float
	    || mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == Id_Not
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::arithExpr:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == Int
	    || mLookAhead.getTokenType() == Float
	    || mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == Id_Not
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::arithExprl:
	if(mLookAhead.getTokenType() == Id_Or
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::sign:
	if(mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
		break;
      case rule::term:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == Int
	    || mLookAhead.getTokenType() == Float
	    || mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == Id_Not
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::terml:
	if(mLookAhead.getTokenType() == Id_And
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Div
	    || mLookAhead.getTokenType() == Mult)
	  return true;
	break;
      case rule::factor:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == Int
	    || mLookAhead.getTokenType() == Float
	    || mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == Id_Not
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::varFuncCall:
      	if(mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == epsilon)
      	  return true;
      	break;
      case rule::variable:
	if(mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::idnestx:
      	if(mLookAhead.getTokenType() == Dot
	    || mLookAhead.getTokenType() == epsilon)
      	  return true;
      	break;
      case rule::idnest:
      	if(mLookAhead.getTokenType() == Dot)
      	  return true;
      	break;
      case rule::indicex:
	if(mLookAhead.getTokenType() == Obra
	    || mLookAhead.getTokenType() == epsilon)
	  return true;
	break;
      case rule::indice:
	if(mLookAhead.getTokenType() == Obra)
	  return true;
	break;
      case rule::arraySizex:
	if(mLookAhead.getTokenType() == Obra
	    || mLookAhead.getTokenType() == epsilon)
	  return true;
	break;
      case rule::arraySize:
	if(mLookAhead.getTokenType() == Obra)
	  return true;
	break;
      case rule::type:
	if(mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::fParams:
	if(mLookAhead.getTokenType() == Id_Int
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Id_Float
	    || mLookAhead.getTokenType() == Id)
	  return true;
	break;
      case rule::aParams:
	if(mLookAhead.getTokenType() == Id
	    || mLookAhead.getTokenType() == epsilon
	    || mLookAhead.getTokenType() == Int
	    || mLookAhead.getTokenType() == Float
	    || mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == Id_Not
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::fParamsTailx:
	if(mLookAhead.getTokenType() == Com
	    || mLookAhead.getTokenType() == epsilon)
	  return true;
	break;
      case rule::fParamsTail:
	if(mLookAhead.getTokenType() == Com)
	  return true;
	break;
      case rule::aParamsTailx:
	if(mLookAhead.getTokenType() == Com
	    || mLookAhead.getTokenType() == epsilon)
	  return true;
	break;
      case rule::aParamsTail:
	if(mLookAhead.getTokenType() == Com)
	  return true;
	break;
      case rule::assignOp:
	if(mLookAhead.getTokenType() == Assig)
	  return true;
	break;
      case rule::relOp:
	if(mLookAhead.getTokenType() == Equal
	    || mLookAhead.getTokenType() == Diff
	    || mLookAhead.getTokenType() == More
	    || mLookAhead.getTokenType() == Less
	    || mLookAhead.getTokenType() == Mot
	    || mLookAhead.getTokenType() == Let)
	  return true;
	break;
      case rule::addOp:
	if(mLookAhead.getTokenType() == Id_Or
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Minus)
	  return true;
	break;
      case rule::multOp:
	if(mLookAhead.getTokenType() == Id_And
	    || mLookAhead.getTokenType() == Div
	    || mLookAhead.getTokenType() == Mult)
	  return true;
	break;
      default:
	return false;
  }
  return false;
}

bool
Parser::follow (rule::Rule_Type rt)
{
  switch(rt)
    {
        case rule::prog:
          if(mLookAhead.getTokenType() == Eof)
	    return true;
	  break;
        case rule::classDeclx:
          if(mLookAhead.getTokenType() == Id_Int
	      || mLookAhead.getTokenType() == Id_Float
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Eof
	      || mLookAhead.getTokenType() == Id_Program)
	    return true;
	  break;
        case rule::classDecl:
          if(mLookAhead.getTokenType() == Id_Class
              ||mLookAhead.getTokenType() == Id_Int
	      || mLookAhead.getTokenType() == Id_Float
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Eof
	      || mLookAhead.getTokenType() == Id_Program)
	    return true;
	  break;
        case rule::varFuncDeclx:
	  if(mLookAhead.getTokenType() == Ckey)
	    return true;
	  break;
	case rule::varFuncDecl:
	  if(mLookAhead.getTokenType() == Id_Int
	      || mLookAhead.getTokenType() == Id_Float
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Ckey)
	    return true;
	  break;
	case rule::typeDef:
	  if(mLookAhead.getTokenType() == Obra
	      || mLookAhead.getTokenType() == Semic
	      || mLookAhead.getTokenType() == Opar)
	    return true;
	  break;
	case rule::varFunc:
	  if(mLookAhead.getTokenType() == Id_Int
	      || mLookAhead.getTokenType() == Id_Float
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Ckey)
	    return true;
	  break;
        case rule::progBody:
          if(mLookAhead.getTokenType() == Eof)
	    return true;
	  break;
        case rule::funcDefx:
          if(mLookAhead.getTokenType() == Eof
	    || mLookAhead.getTokenType() == Id_Program)
	    return true;
	  break;
        case rule::funcDef:
          if(mLookAhead.getTokenType() == Id_Int
	      || mLookAhead.getTokenType() == Id_Float
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Eof
	      || mLookAhead.getTokenType() == Id_Program)
	    return true;
	  break;
        case rule::funcHead:
          if(mLookAhead.getTokenType() == Okey)
	    return true;
	  break;
        case rule::funcBody:
          if(mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::varDeclx:
          if(mLookAhead.getTokenType() == Ckey
	      || mLookAhead.getTokenType() == Id_If
	      || mLookAhead.getTokenType() == Id_For
	      || mLookAhead.getTokenType() == Id_Get
	      || mLookAhead.getTokenType() == Id_Put
	      || mLookAhead.getTokenType() == Id_Return
	      || mLookAhead.getTokenType() == Id)
	    return true;
	  break;
        case rule::varDecl:
          if(mLookAhead.getTokenType() == Id_Int
	      || mLookAhead.getTokenType() == Id_Float
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Ckey
	      || mLookAhead.getTokenType() == Id_If
	      || mLookAhead.getTokenType() == Id_For
	      || mLookAhead.getTokenType() == Id_Get
	      || mLookAhead.getTokenType() == Id_Put
	      || mLookAhead.getTokenType() == Id_Return)
	    return true;
	  break;
        case rule::statementx:
          if(mLookAhead.getTokenType() == Ckey)
	    return true;
	  break;
        case rule::statement:
          if(mLookAhead.getTokenType() == Ckey
	      || mLookAhead.getTokenType() == Id_Else
	      || mLookAhead.getTokenType() == Semic
	      || mLookAhead.getTokenType() == Id_If
	      || mLookAhead.getTokenType() == Id_For
	      || mLookAhead.getTokenType() == Id_Get
	      || mLookAhead.getTokenType() == Id_Put
	      || mLookAhead.getTokenType() == Id_Return
	      || mLookAhead.getTokenType() == Id)
	    return true;
	  break;
        case rule::assignStat:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::statBlock:
          if(mLookAhead.getTokenType() == Id_Else
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::expr:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::pRel:
	  if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::relExpr:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::arithExpr:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::arithExprl:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::sign:
          if(mLookAhead.getTokenType() == Opar
		|| mLookAhead.getTokenType() == Id
		|| mLookAhead.getTokenType() == Int
		|| mLookAhead.getTokenType() == Float
		|| mLookAhead.getTokenType() == Id_Not
		|| mLookAhead.getTokenType() == Minus
		|| mLookAhead.getTokenType() == Plus)
	      return true;
	    break;
        case rule::term:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::terml:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::factor:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Id_And
	      || mLookAhead.getTokenType() == Div
	      || mLookAhead.getTokenType() == Mult
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::varFuncCall:
	  if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Id_And
	      || mLookAhead.getTokenType() == Div
	      || mLookAhead.getTokenType() == Mult
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::variable:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Assig
	      || mLookAhead.getTokenType() == Id_And
	      || mLookAhead.getTokenType() == Div
	      || mLookAhead.getTokenType() == Mult
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::idnestx:
	  if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Opar
	      || mLookAhead.getTokenType() == Assig
	      || mLookAhead.getTokenType() == Id_And
	      || mLookAhead.getTokenType() == Div
	      || mLookAhead.getTokenType() == Mult
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::idnest:
	  if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Opar
	      || mLookAhead.getTokenType() == Dot
	      || mLookAhead.getTokenType() == Assig
	      || mLookAhead.getTokenType() == Id_And
	      || mLookAhead.getTokenType() == Div
	      || mLookAhead.getTokenType() == Mult
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::indicex:
	if(mLookAhead.getTokenType() == Cpar
	    || mLookAhead.getTokenType() == Opar
	    || mLookAhead.getTokenType() == Dot
	    || mLookAhead.getTokenType() == Assig
	    || mLookAhead.getTokenType() == Id_And
	    || mLookAhead.getTokenType() == Div
	    || mLookAhead.getTokenType() == Mult
	    || mLookAhead.getTokenType() == Id_Or
	    || mLookAhead.getTokenType() == Minus
	    || mLookAhead.getTokenType() == Plus
	    || mLookAhead.getTokenType() == Equal
	    || mLookAhead.getTokenType() == Diff
	    || mLookAhead.getTokenType() == Less
	    || mLookAhead.getTokenType() == More
	    || mLookAhead.getTokenType() == Let
	    || mLookAhead.getTokenType() == Mot
	    || mLookAhead.getTokenType() == Cbra
	    || mLookAhead.getTokenType() == Com
	    || mLookAhead.getTokenType() == Semic)
	  return true;
	break;
        case rule::indice:
          if(mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Opar
	      || mLookAhead.getTokenType() == Obra
	      || mLookAhead.getTokenType() == Dot
	      || mLookAhead.getTokenType() == Assig
	      || mLookAhead.getTokenType() == Id_And
	      || mLookAhead.getTokenType() == Div
	      || mLookAhead.getTokenType() == Mult
	      || mLookAhead.getTokenType() == Id_Or
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus
	      || mLookAhead.getTokenType() == Equal
	      || mLookAhead.getTokenType() == Diff
	      || mLookAhead.getTokenType() == Less
	      || mLookAhead.getTokenType() == More
	      || mLookAhead.getTokenType() == Let
	      || mLookAhead.getTokenType() == Mot
	      || mLookAhead.getTokenType() == Cbra
	      || mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::arraySizex:
          if(mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::arraySize:
          if(mLookAhead.getTokenType() == Com
	      || mLookAhead.getTokenType() == Obra
	      || mLookAhead.getTokenType() == Cpar
	      || mLookAhead.getTokenType() == Semic)
	    return true;
	  break;
        case rule::type:
          if(mLookAhead.getTokenType() == Id)
	    return true;
	  break;
        case rule::fParams:
          if(mLookAhead.getTokenType() == Cpar)
	    return true;
	  break;
        case rule::aParams:
          if(mLookAhead.getTokenType() == Cpar)
	    return true;
	  break;
        case rule::fParamsTailx:
          if(mLookAhead.getTokenType() == Cpar)
	      return true;
	  break;
        case rule::fParamsTail:
          if(mLookAhead.getTokenType() == Cpar
              || mLookAhead.getTokenType() == Com)
	    return true;
	  break;
        case rule::aParamsTailx:
          if(mLookAhead.getTokenType() == Cpar)
	    return true;
	  break;
        case rule::aParamsTail:
          if(mLookAhead.getTokenType() == Cpar
		|| mLookAhead.getTokenType() == Com)
	    return true;
	  break;
        case rule::assignOp:
          if(mLookAhead.getTokenType() == Opar
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Int
	      || mLookAhead.getTokenType() == Float
	      || mLookAhead.getTokenType() == Id_Not
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus)
	    return true;
	  break;
        case rule::relOp:
          if(mLookAhead.getTokenType() == Opar
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Int
	      || mLookAhead.getTokenType() == Float
	      || mLookAhead.getTokenType() == Id_Not
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus)
	    return true;
	  break;
        case rule::addOp:
          if(mLookAhead.getTokenType() == Opar
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Int
	      || mLookAhead.getTokenType() == Float
	      || mLookAhead.getTokenType() == Id_Not
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus)
	    return true;
	  break;
        case rule::multOp:
          if(mLookAhead.getTokenType() == Opar
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Int
	      || mLookAhead.getTokenType() == Float
	      || mLookAhead.getTokenType() == Id_Not
	      || mLookAhead.getTokenType() == Minus
	      || mLookAhead.getTokenType() == Plus)
	    return true;
	  break;
        default:
          return false;
    }
  return false;
}

bool
Parser::skipErrors (rule::Rule_Type rt)
{
  if(first(rt)
      || (isEpsilonInFirst(rt) && follow(rt)))
  {
      return true;
  }
  else
  {
      mSs<<"syntax error at " << mLookAhead.getPos()<< " Rule: " << rt <<std::endl;
      while(!first(rt) && !follow(rt))
      {
	  mLookAhead = mLex.nextToken();
	  if(!isEpsilonInFirst(rt) && follow(rt))
	  {
	      return false;
	  }
      }
      return true;
  }
}

bool
Parser::isEpsilonInFirst(rule::Rule_Type rt)
{
  Token_Type aux;
  aux = mLookAhead.getTokenType();
  mLookAhead.setTokenType(epsilon);
  if(first(rt))
  {
      mLookAhead.setTokenType(aux);
      return true;
  }
  mLookAhead.setTokenType(aux);
  return false;
}






























