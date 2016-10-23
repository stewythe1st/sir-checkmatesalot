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

void id_minimax( Chess::State* root, Chess::State* bestAction, int depth );
void minimax( Chess::State* root, Chess::State* bestAction, int depth );
int maxVal( Chess::State * state, int depth, Chess::State * bestAction );
int minVal( Chess::State * state, int depth );

#endif