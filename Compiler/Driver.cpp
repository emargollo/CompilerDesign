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

  std::string filename;
  std::cout<< "Which file would you like to compile?"<<std::endl;
  std::cin >> filename;
  bool t = mParser.parse(filename, true);	//Driver to run parser.
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




















