/*
 * SymbolTable.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Emmanuel
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <vector>
#include <iostream>
#include <string.h>
#include "TableEntry.h"

class TableEntry;

class SymbolTable
{
public:
  SymbolTable ();
  SymbolTable(std::string name, SymbolTable* Tb);

  //void	create(SymbolTable& Tn);
  void				search(std::string identifier, TableEntry *&ptr, bool &found);
  void				insert(std::string identifier, TableEntry *&ptr);
  void				deleteTable();
  void				print();

  SymbolTable*			getPreviousTable();
  std::vector<TableEntry*>	getEntries();
  void				setName(std::string name){mName = name;};
  std::string			getName(){return mName;};



  virtual
  ~SymbolTable ();
private:
  std::string				mName;
  std::vector<TableEntry*>		mEntries;
  SymbolTable*				mPreviousTable;
};

#endif /* SYMBOLTABLE_H_ */
