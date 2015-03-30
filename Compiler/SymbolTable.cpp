/*
 * SymbolTable.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: Emmanuel
 */

#include "SymbolTable.h"

SymbolTable::SymbolTable ()
{
  // TODO Auto-generated constructor stub
  mEntries.clear();
}

SymbolTable*
SymbolTable::getPreviousTable ()
{
  return mPreviousTable;
}

std::vector<TableEntry*>
SymbolTable::getEntries ()
{
  return mEntries;
}

SymbolTable::~SymbolTable ()
{
  mEntries.clear();
}

SymbolTable::SymbolTable (std::string name, SymbolTable* Tb)
{
  mPreviousTable = Tb;
  mName = name;
}

void
SymbolTable::search (std::string identifier, TableEntry*& ptr, bool& found)
{
  found = false;
  for(unsigned int i = 0; i < mEntries.size(); i++)
  {
      if(mEntries.at(i)->getName() == identifier)
      {
	  ptr = mEntries.at(i);
	  found = true;
	  return;
      }
  }
  SymbolTable* tb = this->mPreviousTable;
  while(tb != NULL)
  {
      tb->search(identifier, ptr, found);
      tb = tb->getPreviousTable();
  }
}

void
SymbolTable::insert (std::string identifier, TableEntry *&ptr)
{
  TableEntry* te = new TableEntry(identifier);
  mEntries.push_back(te);
  ptr = te;
  //std::cout<<mEntries.size()<< " "<< mEntries.at(mEntries.size() - 1)->getName()<<std::endl;
  //std::cin.get();
}

void
SymbolTable::deleteTable ()
{
  this->~SymbolTable();
}

void
SymbolTable::print ()
{
  std::cout<<"Table: "<<mName<<std::endl;
  std::vector<SymbolTable*> vs;
  for(unsigned int i = 0; i < mEntries.size(); i++)
  {
     std::cout<< mEntries.at(i)->toString()<<std::endl;
     if(mEntries.at(i)->getLink() != NULL)
       vs.push_back(mEntries.at(i)->getLink());
  }
  std::cout<<std::endl;
  for(unsigned int i = 0; i < vs.size(); i++)
  {
      vs.at(i)->print();
  }
}

















