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
SemanticTranslation::getCode ()
{
  return mMoonCode.str();
}
