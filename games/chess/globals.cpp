/**************************************************************
* globals.cpp
* Definitions for the chess global variables
* CS5400, FS 2016
* Stuart Miller
**************************************************************/


/******************************************************
* Includes
******************************************************/
#include "globals.h"
#include <map>
#include <iostream>

/******************************************************
* Local Variables
******************************************************/
// Local defs of global variables
int kingVal;
int queenVal;
int rookVal;
int bishopVal;
int knightVal;
int pawnVal;
int pawnPenalty;
int mobPenalty;
int	movesThreshold;
int	movesEstimate;
int	maxDepth;

// Definition map
static std::map<std::string, int*> valConvert = {
		{ "kingval",		&kingVal },
		{ "queenval",		&queenVal },
		{ "rookval",		&rookVal },
		{ "knightval",		&knightVal },
		{ "bishopval",		&bishopVal },
		{ "pawnval",		&pawnVal },
		{ "pawnpenalty",	&pawnPenalty },
		{ "mobpenalty",		&mobPenalty },
		{ "movesthreshold",	&movesThreshold },
		{ "movesestimate",	&movesEstimate },
		{ "maxdepth",		&maxDepth }
	};

/**************************************************************
* Set Global
* Sets a global variable according to the passed strings
**************************************************************/
void setGlobal( std::string name, std::string value )
	{
	if( valConvert.find( name ) != valConvert.end() )
		{
		int* numericVal = valConvert[ name ];
		size_t valueSize = value.size();
		std::cout << "  Assigning " << value << " to " << name << std::endl;
		*numericVal = std::stoi( value, &valueSize );
		}

	// Name not recognized
	else
		{
		std::cout << "  Parameter " << name << " not found" << std::endl;
		}
	return;
	}