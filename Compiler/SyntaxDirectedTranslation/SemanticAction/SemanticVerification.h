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

  std::string	checkOperatorTypes(std::string type1, std::string type2, bool& success);
  std::string	checkAssignmentTypes(std::string type1, std::string type2, bool& success);
  std::string	checkVarType(std::string var, SymbolTable *table, bool& success);

  std::string	checkAssigTypes(std::string type1, std::string type2, bool& success);

  std::string	checkReturnType(std::string decl, std::string ret, bool& success);

  std::string	checkParameters(std::string decl, std::string nest, std::vector<std::string> call, SymbolTable* table, bool& success);

  std::string	checkUserType(std::string type, SymbolTable *table, bool& success);
  std::string	checkDoubleDeclaration(std::string name, SymbolTable *table, bool& success);

  std::string	checkVarInsideNest(std::string nest, std::string name, SymbolTable *table, bool& success);

  std::string	checkAmountOfIndices(std::string var, int amount, SymbolTable *table, bool& success);
  std::string	checkAmountOfIndicesInNest(std::string nest, std::string var, int amount, SymbolTable *table, bool& success);

  bool		checkDoubleDeclaration(std::string name, SymbolTable *table);


private:

};

#endif /* SEMANTICVERIFICATION_H_ */
