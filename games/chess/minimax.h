/**************************************************************
* minimax.h
* Declarations for the chess minimax algorithm
* CS5400, FS 2016
* Stuart Miller
**************************************************************/
#ifndef JOUEUR_CHESS_MINIMAX_H
#define JOUEUR_CHESS_MINIMAX_H

/******************************************************
* Includes
******************************************************/
#include "chess.h"


/******************************************************
* Types
******************************************************/
typedef enum { MIN, MAX } MinMax;


/******************************************************
* Definitions
******************************************************/
void getStats( int& p, int& e );
void id_minimax( Chess::State* root, Chess::State* bestAction, int maxDepth, double time );
static void minimax( Chess::State* root, int depth, Chess::State* bestAction );
static int minMaxVal( Chess::State * state, int alpha, int beta, int depth, MinMax m, Chess::State * bestAction );

#endif