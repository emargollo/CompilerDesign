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

bool
SymbolTable::dualEntrySearch ()
{
  bool found = false;
  std::vector<SymbolTable*> vs;
  for(unsigned int i = 0; i < mEntries.size(); i++)
  {
     for(int j = i+1; j < mEntries.size(); j++)
     {
	 if(mEntries.at(i)->getName() == mEntries.at(j)->getName() && i != j)
	 {
	     std::cerr<<"Multiple Declared identifier: "<<mEntries.at(i)->getName()<<" at Scope: "<<mName<<std::endl;
	     found = true;
	 }
     }
     if(mEntries.at(i)->getLink() != NULL)
       vs.push_back(mEntries.at(i)->getLink());
  }
  for(unsigned int i = 0; i < vs.size(); i++)
  {
      bool f2 = vs.at(i)->dualEntrySearch();
      if(!found)
      {
	  found = !f2;
      }

  }
  return !found;
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

std::string
SymbolTable::print ()
{
  std::stringstream ss;
  ss<<"Table: "<<mName<<"{"<<std::endl;
  std::vector<SymbolTable*> vs;
  for(unsigned int i = 0; i < mEntries.size(); i++)
  {
     ss<< mEntries.at(i)->toString()<<std::endl;
     if(mEntries.at(i)->getLink() != NULL)
       vs.push_back(mEntries.at(i)->getLink());
  }
  ss<<"}";
  ss<<std::endl;
  for(unsigned int i = 0; i < vs.size(); i++)
  {
      ss<<vs.at(i)->print();
  }
  return ss.str();
}

















