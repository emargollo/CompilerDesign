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
  bool t = mParser.parse("example.txt", true);	//Driver to run parser.
  if(t)
  {
      std::cout<<"Success!"<<std::endl;
  }

  mSymbolTable = SymbolTable("Global", NULL);

}

int main(int argc, char **argv) {
  Driver d;
  d.runDriver(true, true);
//  std::cin.get();
}




















