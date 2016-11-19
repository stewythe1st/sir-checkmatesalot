/**************************************************************
* globals.h
* Declarations for the chess global variables
* CS5400, FS 2016
* Stuart Miller
**************************************************************/
#ifndef JOUEUR_CHESS_GLOBALS_H
#define JOUEUR_CHESS_GLOBALS_H

/******************************************************
* Includes
******************************************************/
#include <string>

/******************************************************
* Global Variables
******************************************************/
extern int kingVal;
extern int queenVal;
extern int rookVal;
extern int bishopVal;
extern int knightVal;
extern int pawnVal;
extern int pawnPenalty;
extern int mobPenalty;
extern int movesThreshold;
extern int movesEstimate;
extern int maxDepth;
extern int histTableMaxSz;
extern int quiescenceDepth;
extern int useEndGameTables;


/******************************************************
* Function Declarations
******************************************************/
void setGlobal( std::string name, std::string value );
void initGlobals();


#endif