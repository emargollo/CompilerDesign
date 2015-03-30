/*
 * Driver.cpp
 *
 *  Created on: Jan 30, 2015
 *      Author: Emmanuel
 */

#include "Driver.h"

Driver::Driver ()
{
  // TODO Auto-generated constructor stub

}

Driver::~Driver ()
{
  // TODO Auto-generated destructor stub
}

void
Driver::runDriver (bool toFile, bool errorFile)
{
/*  mLex.setFile("example.txt");	//Driver to run the lexical analyzer
  Token tk;
  std::ofstream tkFile;
  std::ofstream erFile;
  if(toFile){

    tkFile.open ("Tokens.txt");
  }
  if(errorFile)
  {

      erFile.open ("errorFile.txt");
  }
  while(tk.getTokenType() != Eof){
    tk = mLex.nextToken();
    std::cout<< tk.toString();
    if(toFile)
    {
      tkFile << tk.toString();
    }
    if(errorFile && tk.getTokenType() == Invalid)
    {
	erFile << "Invalid token: " << tk.toString();
    }
  }
  tkFile.close();
  erFile.close();*/
  bool t3 = mParser.parse("example.txt", true);	//Driver to run parser.
  //std::cout<<t3<<std::endl;

  TableEntry *t, *t2, *t4;
  mSymbolTable = SymbolTable("Global", NULL);


/*
  mSymbolTable.insert("test", t);
  t->addDimension(2);
  t->addDimension(3);
  std::cout<<t->getName()<<std::endl;

  t->setName("test2");
  std::cout<<t->getDimensions()<<std::endl;
  bool f;

  mSymbolTable.search("test2", t2, f);
  if(f)
    std::cout<<t2->getDimensions()<<std::endl;
  mSymbolTable.insert("test", t4);*/
  //mSymbolTable.print();

}

int main(int argc, char **argv) {
  Driver d;
  d.runDriver(true, true);
}




















