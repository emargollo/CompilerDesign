/*
 * SemanticTranslation.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: Emmanuel
 */

#include "SemanticTranslation.h"

SemanticTranslation::SemanticTranslation ()
{
  // TODO Auto-generated constructor stub
  mTempCounter = 1;
}

SemanticTranslation::~SemanticTranslation ()
{
  // TODO Auto-generated destructor stub
}

void
SemanticTranslation::generateEntryDirective ()
{
  mMoonCode<<"entry\n";
}

void
SemanticTranslation::generateTemporaryVar(TableEntry *&entry, SymbolTable* table)
{
  std::stringstream name;
  name<<"t"<<mTempCounter;
  mTempCounter++;
  table->insert(name.str(), entry);
  entry->setKind(kind::Variable);
  entry->setAddress(name.str());
  entry->setStructure(structure::Simple);
  entry->setType("float");
  mMoonCode<<name.str()<<"\t"<<"dw\t0\n";
}

void
SemanticTranslation::generateVariableDeclaration (TableEntry *&entry, SymbolTable* table)
{
  int size = 0;

  std::stringstream name;

  name << table->getName() << entry->getName();
  entry->setAddress(name.str());
  mMoonCode<<name.str()<<"\t";
  if((entry->getType() == "int" || entry->getType() == "float") && entry->getStructure() == structure::Simple)
  {
      mMoonCode<<"dw\t0\n";
  }
  else
  {
      size = calculateSize(entry, table);
      mMoonCode<< "res\t" << size << "\n";
  }

  return;
}

int
SemanticTranslation::calculateSize(TableEntry *&entry, SymbolTable* table)
{
//  std::cout<<type<<std::endl;
  if((entry->getType() == "int" || entry->getType() == "float") && entry->getKind() != kind::Function)
  {
    return 1*entry->getDimensions();
  }
  else if(entry->getKind() == kind::Function)
  {
      int size = 0;
      for(int i = 0; i < entry->getLink()->getEntries().size(); i++)
      {
	  size+= calculateSize(entry->getLink()->getEntries().at(i), entry->getLink());
      }
      return size;
  }
  else
  {
      TableEntry* t;
      bool found;
      int size = 0;
      table->search(entry->getType(), t, found);
      for(int i = 0; i < t->getLink()->getEntries().size(); i++)
      {
	  size+= calculateSize(t->getLink()->getEntries().at(i), t->getLink());
      }
      return size;
  }
  return 0;
}

std::string
SemanticTranslation::generateArithOp(std::string var1, std::string var2, TableEntry*& entry , SymbolTable* table, std::string op)
{
  std::string a, b;
  TableEntry *t;
  bool found;
  if(op == "+"|| op == "-"|| op == "*"|| op == "/")
  {
      table->search(var1, t, found);
      a = t->getAddress();
      table->search(var2, t, found);
      b = t->getAddress();
      mMoonCode<<"lw r1,"<<a<<"(r0)\n";
      mMoonCode<<"lw r2,"<<b<<"(r0)\n";
      mMoonCode<<generateOperator(op)<<"r3,r1,r2\n";
      generateTemporaryVar(entry, table);
      mMoonCode<<"sw "<<entry->getAddress()<<"(r0),r3\n";
  }
  return entry->getName();
}

std::string
SemanticTranslation::generateOperator(std::string op)
{
  if(op == "+")
      return "add ";
  if(op == "-")
      return "sub ";
  if(op == "*")
      return "mul ";
  if(op == "/")
      return "div ";
  return "add ";
}

void
SemanticTranslation::generateAssigOp(std::string var1, std::string var2, SymbolTable* table)
{
  std::string a, b;
  TableEntry *t;
  bool found;

  table->search(var1, t, found);
  a = t->getAddress();
  table->search(var2, t, found);
  b = t->getAddress();
  mMoonCode<<"lw r1,"<<b<<"(r0)\n";
  mMoonCode<<"sw "<<a<<"(r0),r1\n";
}

std::string
SemanticTranslation::generateAssigOp(int var2, TableEntry*& entry, SymbolTable* table)
{
  generateTemporaryVar(entry, table);
  mMoonCode<<"sub r1,r1,r1\n";
  mMoonCode<<"addi r1,r1,"<<var2<<"\n";
  mMoonCode<<"sw "<<entry->getAddress()<<"(r0),r1\n";
  return entry->getName();
}

std::string
SemanticTranslation::getCode ()
{
  return mMoonCode.str();
}





















