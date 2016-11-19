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
static bool initialized = false;
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
int histTableMaxSz;
int quiescenceDepth;
int useEndGameTables;

// Definition map
static std::map<std::string, int*> valConvert = {
		{ "kingval",			&kingVal },
		{ "queenval",			&queenVal },
		{ "rookval",			&rookVal },
		{ "knightval",			&knightVal },
		{ "bishopval",			&bishopVal },
		{ "pawnval",			&pawnVal },
		{ "pawnpenalty",		&pawnPenalty },
		{ "mobpenalty",			&mobPenalty },
		{ "movesthreshold",		&movesThreshold },
		{ "movesestimate",		&movesEstimate },
		{ "maxdepth",			&maxDepth },
		{ "histtablemaxsz",		&histTableMaxSz },
		{ "quiescencedepth",	&quiescenceDepth },
		{ "useendgametables",	&useEndGameTables }
	};


/**************************************************************
* Set Global
* Sets a global variable according to the passed strings
**************************************************************/
void setGlobal( std::string name, std::string value )
	{
	if( !initialized )
		{
		initGlobals();
		}
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

/**************************************************************
* Initialize Globals
* Sets all global variables to pre-established default values.
* These are backup values in case a user edits the cfg file
* and accidentally removes a value assignment.
**************************************************************/
void initGlobals()
	{
	kingVal = 2000;
	queenVal = 90;
	rookVal = 50;
	bishopVal = 30;
	knightVal = 30;
	pawnVal = 10;
	pawnPenalty = 5;
	mobPenalty = 1;
	movesThreshold = 40;
	movesEstimate = 150;
	maxDepth = 20;
	histTableMaxSz = 100000;
	quiescenceDepth = 2;
	useEndGameTables = 0;
	initialized = true;
	}