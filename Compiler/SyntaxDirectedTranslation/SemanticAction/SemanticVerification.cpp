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
  mError<<"Operations between: "<<type1<< " and: " << type2<< " are not allowed"<<std::endl;
  success = false;
  return "Operation not allowed";
}

std::string
SemanticVerification::checkAssigTypes (std::string type1, std::string type2, bool& success)
{
  if((type1 == "int" || type1 == "float") &&
      (type2 == "int" || type2 == "float")){
      return type1;
  }
  else if(type1 == type2)
  {
      return type1;
  }
  mError<<"Can't Convert: "<<type2<< " into: " << type1 <<std::endl;
  success = false;
  return "Operation not allowed";
}

std::string
SemanticVerification::checkReturnType(std::string decl, std::string ret, bool& success)
{
  if((decl == "int" || decl == "float") &&
      (ret == "int" || ret == "float")){
      return decl;
  }
  else if(decl == ret)
  {
      return decl;
  }
  mError<<"Can't Return: "<<ret<< " in a : " << decl << " Function" <<std::endl;
  success = false;
  return "Operation not allowed";
}

std::string
SemanticVerification::checkParameters(std::string decl, std::string nest, std::vector<std::string> call, SymbolTable* table, bool& success)
{
  std::string type;
  TableEntry *t;
  bool found;
  if(nest != "null")
  {
      table->search(nest, t, found);
      if(found)
      {
	  nest = t->getType();
	  table->search(nest, t, found);
	  if(t->getLink() != NULL)
	  {
	      table = t->getLink();
	  }
	  else
	  {
	    success = false;
	    mError<<"Use of Variable as a Nest: "<<nest<<std::endl;
	    return "Not a Nest";
	  }
      }
      else
      {
          success = false;
          mError<<"Use of Nest not Declared: "<<nest<<std::endl;
          return "Function Nest not found";
      }

  }
  table->search(decl, t, found);
  if(found)
  {
      if(t->getParameters().size() == call.size())
      {
	  for(int i = 0; i < call.size(); i++)
	  {
	      bool check;
	      checkAssigTypes(checkAmountOfIndices(t->getLink()->getEntries().at(i)->getName(), 0, t->getLink(), check), call.at(i), success);
	  }
	  return "All parameters are correct";
      }
      else
      {
	  success = false;
	  mError<<"Incorrect amout of Parameters for function: "<<decl<<std::endl;
	  return "Incorrect amout of Parameter";
      }
  }
  else
  {
      success = false;
      mError<<"Use of Function not Declared: "<<decl<<std::endl;
      return "Variable not found";
  }


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
      mError<<"Use of Variable not Declared: "<<var<<std::endl;
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
  mError<<"Undefined User Type used."<<std::endl;
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
	  mError<<"Multiple Declared identifier: "<<table->getEntries().at(i)->getName()<<" at Scope: "<<table->getName()<<std::endl;
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
	  //mError<<"Multiple Declared identifier: "<<table->getEntries().at(i)->getName()<<" at Scope: "<<table->getName()<<std::endl;
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
	       success = false;
	       mError<<"Variable not found in Nest: "<< nest<< "."<< name<<std::endl;
	       return "Variable not found";
	   }
       }
       else
       {
	   success = false;
	   mError<<"Use of Variable as a Nest: "<<nest<<std::endl;
	   return "Not a nest";
       }

   }
   else
   {
       success = false;
       mError<<"Use of Nest not Declared: "<<nest<<std::endl;
       return "Variable not found";
   }
}

std::string
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
	  return t->getType();
      }
      else
      {
	  //mError<<"Amount of Indices different from variable Declaration: "<<var<<std::endl;
	  std::stringstream ss;
	  ss<<"array"<< t->getDimensions();
	  return ss.str();
      }
  }
  else
  {
      success = false;
      mError<<"Use of Variable not Declared: "<<var<<std::endl;
      return "Variable not found";
  }
}

std::string
SemanticVerification::checkAmountOfIndicesInNest(std::string nest, std::string var, int amount, SymbolTable *table, bool& success)
{
  std::string type;
  TableEntry *t, *t2;
  bool found;
  table->search(var, t, found);

  table->search(nest, t, found);
  if(found)
  {
      nest = t->getType();
      table->search(nest, t, found);
      if(t->getLink() != NULL)
      {
	 t->getLink()->search(var, t2, found);
	 if(found)
	 {
	     if(t2->getDimension().size() == amount)
	     {
		return t2->getType();
	     }
	     else
	     {
		//mError<<"Amount of Indices different from variable Declaration: "<<var<<std::endl;
		 std::stringstream ss;
		 ss<<"array"<< t->getDimension().size()-amount;
		 return ss.str();
	     }
	 }
	 else
	 {
	     success = false;
	     mError<<"Variable not found in Nest: "<< nest<< "."<< var<<std::endl;
	     return "Variable not found";
	 }
     }
     else
     {
       success = false;
       mError<<"Use of Variable as a Nest: "<<nest<<std::endl;
       return "Not a nest";
     }
  }
  else
  {
       success = false;
       mError<<"Use of Nest not Declared: "<<nest<<std::endl;
       return "Variable not found";
  }

}

std::string
SemanticVerification::getErrors ()
{
  return mError.str();
}
