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
* Compiler Constants
******************************************************/
#define NS_PER_MS			( 1000000 )
#define TIME_TOLERANCE		( 10 )


/******************************************************
* Types
******************************************************/
typedef enum { MIN, MAX } MinMax;


/******************************************************
* Definitions
******************************************************/
void getStats( int& p, int& e, int& enq, int & d );
void id_minimax( Chess::State* root, Chess::State* bestAction, double time );
static void minimax( Chess::State* root, int depth, int qDepth, Chess::State* bestAction );
static int minMaxVal( Chess::State * state, int alpha, int beta, int depth, int qDepth, MinMax m, Chess::State * bestAction );

#endif