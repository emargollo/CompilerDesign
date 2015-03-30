/*
 * Driver.h
 *
 *  Created on: Jan 30, 2015
 *      Author: Emmanuel
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include "Parser.h"
#include "SymbolTable.h"
#include <fstream>

class Driver
{
public:
  Driver ();
  virtual
  ~Driver ();

  void	runDriver(bool toFile, bool errorFile);

private:
  LexicalAnalyzer mLex;
  Parser	  mParser;
  SymbolTable	  mSymbolTable;
};

#endif /* DRIVER_H_ */
