/*
 * TableEntry.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Emmanuel
 */

#ifndef TABLEENTRY_H_
#define TABLEENTRY_H_

#include "SymbolTable.h"
#include "sstream"

namespace kind{
  typedef enum kind{
    Variable,
    Parameter,
    Function,
    Class
  }kind;
}
namespace structure{
  typedef enum structure{
    Simple,
    Array,
    Class
  }structure;
}

class SymbolTable;

class TableEntry
{
public:
  TableEntry ();
  TableEntry(std::string name);
  virtual
  ~TableEntry ();
  void  setAddress (int address);
  void  setKind (kind::kind kind);
  void  setLink (SymbolTable* link);
  void  setName (std::string n);
  void  setStructure (structure::structure structure);
  void	setType(std::string type){mType = type;};
  void	addDimension(int i);
  void	addParameter(std::string param);

  int				getDimensions();
  int  				getAddress ();
  std::vector<int>  		getDimension ();
  kind::kind  			getKind ();
  SymbolTable*  		getLink ();
  std::string  			getName ();
  std::vector<std::string>  	getParameters ();
  structure::structure  	getStructure ();
  std::string  			getType ();

  std::string			toString();

private:
  std::string			mName;
  std::string			mType;
  std::vector<std::string>	mParameters;
  kind::kind			mKind;
  structure::structure		mStructure;
  std::vector<int>		mDimension;
  int				mAddress;
  SymbolTable* 			mLink;

  std::string			kindToString(kind::kind);
  std::string			strucToString(structure::structure);
  std::string			printParams();
  std::string			printDimensions();
};

#endif /* TABLEENTRY_H_ */
