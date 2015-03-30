/*
 * TableEntry.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: Emmanuel
 */

#include "TableEntry.h"

TableEntry::TableEntry ()
{
  // TODO Auto-generated constructor stub

}

TableEntry::TableEntry (std::string name)
{
  mName = name;
  mAddress = 0;
  mKind = kind::Variable;
  mType = "int";
  mStructure = structure::Simple;
  mLink = NULL;
}

TableEntry::~TableEntry ()
{
  // TODO Auto-generated destructor stub
}

void
TableEntry::setAddress (int address)
{
  mAddress = address;
}

void
TableEntry::setKind (kind::kind kind)
{
  mKind = kind;
}

void
TableEntry::setLink (SymbolTable* link)
{
  mLink = link;
}

void
TableEntry::setName (std::string n)
{
  mName = n;
}

void
TableEntry::setStructure (structure::structure structure)
{
  mStructure = structure;
}

void
TableEntry::addDimension (int i)
{
  mDimension.push_back(i);
}

void
TableEntry::addParameter (std::string param)
{
  mParameters.push_back(param);
}

int
TableEntry::getDimensions ()
{
  int d = 1;
  for(unsigned int i = 0; i < mDimension.size(); i++)
  {
    d *= mDimension.at(i);
  }
  return d;
}

int
TableEntry::getAddress ()
{
  return mAddress;
}

std::vector<int>
TableEntry::getDimension ()
{
  return mDimension;
}

kind::kind
TableEntry::getKind ()
{
  return mKind;
}

SymbolTable*
TableEntry::getLink ()
{
  return mLink;
}

std::string
TableEntry::getName ()
{
  return mName;
}

std::vector<std::string>
TableEntry::getParameters ()
{
  return mParameters;
}

structure::structure
TableEntry::getStructure ()
{
  return mStructure;
}

std::string
TableEntry::getType ()
{
  return mType;
}

std::string
TableEntry::toString()
{
  return mName;
}
