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
  else
    success = false;
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
  }
  if(success)
  {
      success = mTableHead->dualEntrySearch();
  }
  std::cout<<mTableHead->print();
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
	  mCurrentEntry->setName(mLookAhead.getLexeme());
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
      if(typeDef() && varFunc())
      {
	  mSs<< "<varFuncDecl> -> <typeDef><varFunc>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::typeDef ()
{
  bool success = skipErrors(rule::typeDef);

  if(first(rule::type))
  {
      std::string typeDefTC;
      mCurrentTable->insert("variable/function", mCurrentEntry);		//Enter class entry in table
      //mCurrentEntry->setType(mLookAhead.getLexeme());

      if(type(typeDefTC)){
	  mCurrentEntry->setName(mLookAhead.getLexeme());
	  if(match(Id))
	  {
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
Parser::varFunc ()
{
  bool success = skipErrors(rule::varFunc);
  if(first(rule::arraySizex) || first(Semic))
  {
      mCurrentEntry->setKind(kind::Variable);
      mCurrentEntry->setStructure(structure::Simple);
      if(arraySizex() && match(Semic))
      {
	mSs<< "<varFunc> -> <arraySize*>;"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Opar))
  {
      createTable(std::string(mCurrentTable->getName() + ":" + mCurrentEntry->getName()));

      mCurrentEntry->setKind(kind::Function);
      mCurrentEntry->setStructure(structure::Simple);
      mCurrentEntry->setLink(mCurrentTable);

      mFunctionEntry = mCurrentEntry;
      if(match(Opar) && fParams() && match(Cpar) && funcBody() && match(Semic))
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
  if(first(Id_Program))
  {
      mCurrentTable->insert("Program", mCurrentEntry);

      createTable("Program");

      mCurrentEntry->setKind(kind::Function);
      mCurrentEntry->setStructure(structure::Simple);
      mCurrentEntry->setType(mLookAhead.getLexeme());
      mCurrentEntry->setLink(mCurrentTable);
      if(match(Id_Program) && funcBody() && match(Semic) ){
	  if(funcDefx())
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
  bool success = skipErrors(rule::funcDef);
  if(first(rule::funcHead))
  {
      if(funcHead() && funcBody() && match(Semic))
      {
	  mSs<< "<funcDef> -> <funcHead><funcBody>;"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::funcHead ()
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
	  mCurrentEntry->setName(mLookAhead.getLexeme());
	  mFunctionEntry = mCurrentEntry;
	  if(match(Id) && match(Opar) && fParams() && match(Cpar))
	  {
	    mSs<< "<funcHead>	-> <type>id(<fParams>)"<<std::endl;

	  }
	  else{success = false;}
      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::funcBody ()
{
  bool success = skipErrors(rule::funcBody);
  if(first(Okey))
  {
      if(match(Okey) && varDeclx() && statementx() && match(Ckey))
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
      std::string varDeclTC;
      mCurrentTable->insert("variable", mCurrentEntry);
      //mCurrentEntry->setType(mLookAhead.getLexeme());
      mCurrentEntry->setKind(kind::Variable);
      mCurrentEntry->setStructure(structure::Simple);
      if(type(varDeclTC) ){
	  mCurrentEntry->setType(varDeclTC);
	  mCurrentEntry->setName(mLookAhead.getLexeme());
	  if(match(Id) && arraySizex() && match(Semic))
	  {
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
Parser::statementx ()
{
  bool success = skipErrors(rule::statementx);
  if(first(rule::statement))
  {
      if(statement() && statementx())
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
Parser::statement ()
{
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
      if(match(Id_If) && match(Opar) && expr() && match(Cpar) && match(Id_Then)
	  && statBlock() && match(Id_Else) && statBlock() && match(Semic))
      {
	  mSs<< "<statement> -> if(<expr>)then<statBlock>else<statBlock>;"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_For))
  {
      if(match(Id_For) && match(Opar) && type() && match(Id) && assignOp() &&
	  expr() && match(Semic) && relExpr() && match(Semic) && assignStat()
	  && match(Cpar) && statBlock() && match(Semic))
      {
	  mSs<< "<statement> -> for(<type>id<assignOp><expr>;<relExpr>;<assignStat>)<statBlock>;"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Get))
  {
      if(match(Id_Get) && match(Opar) && variable() && match(Cpar) && match(Semic))
      {
	  mSs<< "<statement> -> get(<variable>);"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Put))
  {
      if(match(Id_Put) && match(Opar) && expr() && match(Cpar) && match(Semic))
      {
	  mSs<< "<statement> -> put(<expr>);"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Return))
  {
      if(match(Id_Return) && match(Opar) && expr() && match(Cpar) && match(Semic))
      {
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
  bool success = skipErrors(rule::assignStat);
  if(first(rule::variable))
  {
      if(variable() && assignOp() && expr())
      {
	  mSs<< "<assignStat> -> <variable><assignOp><expr>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::statBlock ()
{
  bool success = skipErrors(rule::statBlock);
  if(first(Okey))
  {
      if(match(Okey) && statementx() && match(Ckey))
      {
	  mSs<< "<statBlock> -> {<statement*>} "<<std::endl;

      }

  }
  else if(first(rule::statement))
  {
      if(statement())
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
Parser::expr ()
{
  bool success = skipErrors(rule::expr);
  if(first(rule::arithExpr))
  {
      if(arithExpr() && pRel())
      {
	  mSs<< "<expr> -> <arithExpr><pRel>" <<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::pRel ()
{
  bool success = skipErrors(rule::pRel);
  if(first(rule::relOp))
  {
      if(relOp() && arithExpr())
      {
	  mSs<< "<pRel> -> <relOp><arithExpr>" <<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::pRel))
  {
      mSs<< "<pRel> -> epsilon" <<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::relExpr ()
{
  bool success = skipErrors(rule::relExpr);
  if(first(rule::arithExpr))
  {
      if(arithExpr() && relOp() && arithExpr())
      {
	  mSs<< "<relExpr> -> <arithExpr><relOp><arithExpr>" <<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::arithExpr ()
{
  bool success = skipErrors(rule::arithExpr);
  if(first(rule::term))
  {
      if(term() && arithExprl())
      {
	  mSs<< "<arithExpr> -> <term><arithExpr'>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::arithExprl ()
{
  bool success = skipErrors(rule::arithExprl);
  if(first(rule::addOp))
  {
      if(addOp() && term() && arithExprl())
      {
	  mSs<< "<arithExpr'> -> <addOp><term><arithExpr'>" <<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::arithExprl))
  {
      mSs<< "<arithExpr'> -> epsilon" <<std::endl;

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
Parser::term ()
{
  bool success = skipErrors(rule::term);
  if(first(rule::factor))
  {
      if(factor() && terml())
      {
	  mSs<< "<term> -> <factor><term'>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::terml ()
{
  bool success = skipErrors(rule::terml);
  if(first(rule::multOp))
  {
      if(multOp() && factor() && terml())
      {
	  mSs<< "<term'> -> <multOp><factor><term'>" <<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::terml))
  {
      mSs<< "<term'> -> epsilon" <<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::factor ()
{
  bool success = skipErrors(rule::factor);
  if(first(Id))
  {
      if(match(Id) && indicex() &&  idnestx() && varFuncCall())
      {
	  mSs<< "<factor>	-> id<idnest*><varFuncCall>"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Float))
  {
      if(match(Float))
      {
	  mSs<< "<factor>	-> num"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Int))
  {
      if(match(Int))
      {
	mSs<< "<factor> -> num"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Opar))
  {
      if(match(Opar) && arithExpr() && match(Cpar)	)
      {
	mSs<< "<factor> -> (<arithExpr>)"<<std::endl;

      }
      else{success = false;}
  }
  else if(first(Id_Not))
  {
      if(match(Id_Not) && factor())
      {
	mSs<< "<factor> -> not<factor>"<<std::endl;

      }
      else{success = false;}
  }
  else if(rule::sign)
  {
      if(sign() && factor())
      {
      	mSs<< "<factor> -> <sign><factor>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::varFuncCall()
{
  bool success = skipErrors(rule::varFuncCall);
  if(first(Opar))
  {
      if(match(Opar) && aParams() && match(Cpar))
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
Parser::variable ()
{
  bool success = skipErrors(rule::variable);
  if(first(Id))
  {
      if(match(Id) && indicex() && idnestx())
      {
	  mSs<< "<variable> -> id<indice*><idnest*>"<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::idnestx ()
{
  bool success = skipErrors(rule::idnestx);
  if(first(rule::idnest))
  {
      if(idnest() && idnestx())
      {
      mSs<<"<idnest*> -> <idnest><idnest*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::idnestx))
  {
  mSs<<"<idnest*> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::idnest ()
{
  bool success = skipErrors(rule::idnest);
  if(first(Dot))
  {
      if(match(Dot) && match(Id) && indicex())
      {
	  mSs<< "<idnest> -> id<indice*>."<<std::endl;

      }
      else{success = false;}
  }
  else{success = false;}
  return(success);
}

bool
Parser::indicex ()
{
  bool success = skipErrors(rule::indicex);
  if(first(rule::indice))
  {
      if(indice() && indicex())
      {
      mSs<<"<indice*> -> <indice><indice*> "<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::indicex))
  {
  mSs<<"<indice*> -> epsilon"<<std::endl;

  }
  else{success = false;}
  return(success);
}

bool
Parser::indice ()
{
  bool success = skipErrors(rule::indice);
  if(first(Obra))
  {
      if(match(Obra) && arithExpr() && match(Cbra))
      {
	  mSs<< "<indice>	-> [<arithmExpr>]"<<std::endl;

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
      type = mSeV.checkUserType(mLookAhead.getLexeme(), mCurrentTable);
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
	mCurrentEntry->setName(mLookAhead.getLexeme());
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
Parser::aParams ()
{
  bool success = skipErrors(rule::aParams);
  if(first(rule::expr))
  {
      if(expr() && aParamsTailx())
      {
	  mSs<< "<aParams> -> <expr><aParamsTail*>"<<std::endl;

      }
      else{success = false;}
  }
  else if(follow(rule::aParams))
  {
      mSs<< "<aParams> -> epsilon"<<std::endl;

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
	    mCurrentEntry->setName(mLookAhead.getLexeme());
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
  bool success = skipErrors(rule::aParamsTail);
  if(first(Com))
  {
      if(match(Com) && expr())
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
Parser::relOp ()
{
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
Parser::addOp ()
{
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
Parser::multOp ()
{
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
	    || mLookAhead.getTokenType() == Id_Program)
	  return true;
	break;
      case rule::classDeclx:
	if(mLookAhead.getTokenType() == Id_Class
	    || mLookAhead.getTokenType() == epsilon)
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
	if(mLookAhead.getTokenType() == Id_Program)
	  return true;
	break;
      case rule::funcDefx:
	if(mLookAhead.getTokenType() == Id_Int
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
          if(mLookAhead.getTokenType() == Id_Program)
	    return true;
	  break;
        case rule::classDecl:
          if(mLookAhead.getTokenType() == Id_Class
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
          if(mLookAhead.getTokenType() == Eof)
	    return true;
	  break;
        case rule::funcDef:
          if(mLookAhead.getTokenType() == Id_Int
	      || mLookAhead.getTokenType() == Id_Float
	      || mLookAhead.getTokenType() == Id
	      || mLookAhead.getTokenType() == Eof)
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






























