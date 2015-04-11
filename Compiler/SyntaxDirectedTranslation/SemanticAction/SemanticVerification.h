/*
 * SemanticVerification.h
 *
 *  Created on: Apr 11, 2015
 *      Author: Emmanuel
 */

#ifndef SEMANTICVERIFICATION_H_
#define SEMANTICVERIFICATION_H_

#include <vector>
#include <iostream>
#include <string.h>
#include "../SymbolTable.h"

class SemanticVerification
{
public:
  SemanticVerification ();
  virtual
  ~SemanticVerification ();

  std::string	checkOperatorTypes(std::string type1, std::string type2);
  std::string	checkVarType(std::string var, SymbolTable *table);

  std::string	checkUserType(std::string type, SymbolTable *table);


private:

};

#endif /* SEMANTICVERIFICATION_H_ */
