/**************************************************************
* minimax.h
* Definitions for the chess minimax algorithm
* CS5400, FS 2016
* Stuart Miller
**************************************************************/


/******************************************************
* Includes
******************************************************/
#include "ai.h"
#include "chess.h"
#include "state.h"
#include "minimax.h"


/******************************************************
* Iterative Deepening Minimax Root Call
******************************************************/
void id_minimax( Chess::State* root, Chess::State* bestAction, int depth )
	{
	for( int i = 1; i <= depth; i++ )
		{
		minimax( root, bestAction, i );
		}
	return;
	}


/******************************************************
* Minimax Root Call
******************************************************/
void minimax( Chess::State* root, Chess::State* bestAction, int depth )
	{
	maxVal( root, depth, bestAction );
	return;
	}


/******************************************************
* MaxVal
* Returns the maximum value of the passed state's children
* If the passed state pointer is non-null, it will also
* return a pointer to the maximum valued state
******************************************************/
int maxVal( Chess::State* state, int depth, Chess::State* bestAction )
	{
	// Check depth limit
	if( depth == 0 )
		{
		return state->score;
		}		

	// Declarations
	std::vector<Chess::State*>	frontier;
	int							val;
	int							bestVal = INT_MIN;

	// Build frontier
	state->Actions( frontier, ME );

	// Evaluate each state in frontier
	for( std::vector<Chess::State*>::iterator runner = frontier.begin(); runner != frontier.end(); runner++ )
		{
		val = minVal( *runner, depth - 1 );
		( *runner )->score = val;
		if( val > bestVal )
			bestVal = val;
		}

	// Free memory (and return if root call)
	for( int i = 0; i < frontier.size(); i++ )
		{
		if( bestAction != nullptr && frontier[ i ]->score == bestVal )
			{
			*bestAction = *frontier[ i ];
			return bestVal;
			}
			
		else
			delete frontier[ i ];
		}

	// Return value
	return bestVal;
	}


/******************************************************
* MinVal
* Returns the minimum value of the passed state's children
******************************************************/
int minVal( Chess::State* state, int depth )
	{
	// Check depth limit
	if( depth == 0 )
		{
		return state->score;
		}

	// Declarations
	std::vector<Chess::State*>	frontier;
	int							val;
	int							bestVal = INT_MAX;

	// Build frontier
	state->Actions( frontier, OPPONENT );

	// Evaluate each state in frontier
	for( std::vector<Chess::State*>::iterator runner = frontier.begin(); runner != frontier.end(); runner++ )
		{
		val = maxVal( *runner, depth - 1, nullptr );
		( *runner )->score = val;
		if( val < bestVal )
			bestVal = val;
		}

	// Free memory
	for( int i = 0; i < frontier.size(); i++ )
		delete frontier[ i ];

	// Return value
	return bestVal;
	}


