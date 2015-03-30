/*
 * Table.h
 *
 *  Created on: Jan 25, 2015
 *      Author: Emmanuel
 */

#ifndef TABLE_H_
#define TABLE_H_

#include "Token.h"
#include "stdio.h"
#include <vector>
#include <string>

#define size_x 43
#define size_y 27

#define final 24
#define types 25
#define backtrack 26

typedef enum Table_Aux
{
	Yes,
	No,
	BT_to_int,
	BT_to_float,
	Final
}Table_Aux;

class Table
{
public:
	Table();
	virtual ~Table();
	bool			isFinalState(int state);
	Token_Type		getFinalType(int state);
	int			nextState(int state, char c);
	Table_Aux		isBackTrack(int state);
private:
	unsigned int		getValueRepresented(char c);
	int			mTable[size_x][size_y];
};

#endif /* TABLE_H_ */
