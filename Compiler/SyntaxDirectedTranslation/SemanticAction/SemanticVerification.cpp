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
  if((type1 == "int" || type1 == "float") &&
      (type2 == "int" || type2 == "float")){
      return "float";
  }
  std::cerr<<"Operations between: "<<type1<< " and: " << type2<< " are not allowed"<<std::endl;
  success = false;
  return "Operation not allowed";
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
      success = false;
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

bool
SemanticVerification::checkDoubleDeclaration (std::string name, SymbolTable* table)
{
  bool success = true;
  for(int i = 0; i < table->getEntries().size(); i++)
  {
      if(table->getEntries().at(i)->getName() == name)
      {
	  success = false;
	  //std::cerr<<"Multiple Declared identifier: "<<table->getEntries().at(i)->getName()<<" at Scope: "<<table->getName()<<std::endl;
	  return success;
      }
  }
  return success;
}

std::string
SemanticVerification::checkVarInsideNest(std::string nest, std::string name, SymbolTable *table, bool& success)
{
   std::string type;
   TableEntry *t, *t2;
   bool found;
   table->search(nest, t, found);
   if(found)
   {
       nest = t->getType();
       table->search(nest, t, found);
       if(t->getLink() != NULL)
       {
	   t->getLink()->search(name, t2, found);
	   if(found)
	   {
	       return t2->getType();
	   }
	   else
	   {
	       std::cerr<<"Variable not found in Nest: "<< nest<< "."<< name<<std::endl;
	       return "Variable not found";
	   }
       }
       else
       {
	   std::cerr<<"Use of Variable as a Nest: "<<nest<<std::endl;
	   return "Not a nest";
       }

   }
   else
   {
       std::cerr<<"Use of Nest not Declared: "<<nest<<std::endl;
       return "Variable not found";
   }
}

void
SemanticVerification::checkAmountOfIndices(std::string var, int amount, SymbolTable *table, bool& success)
{
  std::string type;
  TableEntry *t;
  bool found;
  table->search(var, t, found);
  if(found)
  {
      if(t->getDimension().size() == amount)
      {
	  return;
      }
      else
      {
	  std::cerr<<"Amount of Indices different from variable Declaration: "<<var<<std::endl;
      }
  }
  else
  {
      std::cerr<<"Use of Variable not Declared: "<<var<<std::endl;
  }
}
















