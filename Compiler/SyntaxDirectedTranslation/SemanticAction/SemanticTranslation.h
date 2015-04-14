/*
 * SemanticTranslation.h
 *
 *  Created on: Apr 11, 2015
 *      Author: Emmanuel
 */

#ifndef SEMANTICTRANSLATION_H_
#define SEMANTICTRANSLATION_H_


#include <vector>
#include <iostream>
#include <string.h>
#include "../SymbolTable.h"

class SemanticTranslation
{
public:
  SemanticTranslation ();
  virtual
  ~SemanticTranslation ();

  void 		generateEntryDirective();
  void		generateTemporaryVar(TableEntry *&entry, SymbolTable* table);
  void  	generateVariableDeclaration(TableEntry *&entry, SymbolTable* table);
  int		calculateSize(TableEntry *&entry, SymbolTable* table);

  std::string	getCode();

private:
  std::stringstream mMoonCode;
  int	mTempCounter;
};

#endif /* SEMANTICTRANSLATION_H_ */
