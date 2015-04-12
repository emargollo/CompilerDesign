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
SemanticVerification::checkOperatorTypes (std::string type1, std::string type2, bool& success)
{

  return "";
}

std::string
SemanticVerification::checkVarType (std::string var, SymbolTable* table, bool& success)
{
  std::string type;
  TableEntry *t;
  bool found;
  table->search(var, t, found);
  if(found)
  {
      return t->getType();
  }
  else
  {
      std::cerr<<"Use of Variable not Declared: "<<var<<std::endl;
      return "Variable not found";
  }
}

std::string
SemanticVerification::checkUserType (std::string type, SymbolTable* table, bool& success)
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
  std::cerr<<"Undefined User Type used."<<std::endl;
  success = false;
  return "Undefined Type";
}

std::string
SemanticVerification::checkDoubleDeclaration (std::string name, SymbolTable* table, bool& success)
{
  for(int i = 0; i < table->getEntries().size(); i++)
  {
      if(table->getEntries().at(i)->getName() == name)
      {
	  success = false;
	  std::cerr<<"Multiple Declared identifier: "<<table->getEntries().at(i)->getName()<<" at Scope: "<<table->getName()<<std::endl;
	  return "Multiple Declaration";
      }
  }
  return name;
}


















