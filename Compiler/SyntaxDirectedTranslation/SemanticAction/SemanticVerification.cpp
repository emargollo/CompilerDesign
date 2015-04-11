/*
 * SemanticVerification.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: Emmanuel
 */

#include "SemanticVerification.h"

SemanticVerification::SemanticVerification ()
{
  // TODO Auto-generated constructor stub

}

SemanticVerification::~SemanticVerification ()
{
  // TODO Auto-generated destructor stub
}

std::string
SemanticVerification::checkOperatorTypes (std::string type1, std::string type2)
{
  return "..";
}

std::string
SemanticVerification::checkVarType (std::string var, SymbolTable* table)
{
  return "..";
}

std::string
SemanticVerification::checkUserType (std::string type, SymbolTable* table)
{
  TableEntry *t;
  bool found;
  table->search(type, t, found);
  if(found)
  {
      if(t->getKind() == kind::Class)
      {
	  return type;
      }
  }
  else
  {
      return "Undefined Type";
  }
}




















