/*
 * LexicalAnalizer.h
 *
 *  Created on: Jan 25, 2015
 *      Author: Emmanuel
 */

#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

#include "Table.h"
#include "stdio.h"

class LexicalAnalyzer
{
public:
	LexicalAnalyzer();
	virtual ~LexicalAnalyzer();

	Token		nextToken();
	void		setFile(std::string file);

private:
	void		backTrack(Table_Aux bt);
	char		nextChar();
	std::string	getLexeme(int tkpos);
	Token*		createToken(int tkPos, Token_Type tkt);
	Token_Type	checkReservedIds(std::string id);

	Table		mTable;
	int		mCurrentPos;
	std::string	mFileName;
};

#endif /* LEXICALANALYZER_H_ */
