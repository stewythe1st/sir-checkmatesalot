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
#include "globals.h"
#include <algorithm>
#include <time.h>
#include <unordered_map>


/******************************************************
* Macros
******************************************************/
#define MAX( x, y )		( ( x ) > ( y ) ? ( x ) : ( y ) )
#define MIN( x, y )		( ( x ) < ( y ) ? ( x ) : ( y ) )


/******************************************************
* Variables
******************************************************/
static int			pruned;
static int			expanded;
static int			depth;
static clock_t		endTime;
static int			moves = 0;
std::unordered_map<Chess::State, int, StateHash> 
					historyTable;


/******************************************************
* Return Statistics
******************************************************/
void getStats( int& p, int& e, int& d )
	{
	p = pruned;
	e = expanded;
	d = depth;
	return;
	}


/******************************************************
* Iterative Deepening Minimax Root Call
* time is given in ns
******************************************************/
void id_minimax( Chess::State* root, Chess::State* bestAction, double time )
	{

	// Update vars
	moves++;
	pruned = 0;
	expanded = 0;

	// Calculate allowed time
	if( moves > ( movesEstimate - movesThreshold ) )
		{
		movesEstimate++;
		}
	endTime = clock() + ( time / NS_PER_SEC / movesEstimate * CLOCKS_PER_SEC );

	// Iteratively call minimax
	int toIdx, fromIdx;
	Chess::State fallbackAction;
	for( depth = 1; depth < maxDepth; depth++ )
		{
		fallbackAction = *bestAction;
		std::cout << "  Depth " << depth << ": ";
		minimax( root, depth, bestAction );
		if( clock() > endTime )
			{
			*bestAction = fallbackAction;
			std::cout << "Ran out of time!" << std::endl;
			break;
			}
		else
			{
			toIdx = ( bestAction->misc.to_ullong() & TOIDX_MASK ) >> TOIDX_BITSHIFT;
			fromIdx = ( bestAction->misc.to_ullong() & FROMIDX_MASK ) >> FROMIDX_BITSHIFT;
			std::cout << "Chose " << ( char )( ( fromIdx % 8 ) + 'a' ) << ( fromIdx / 8 ) + 1 << " to " << ( char )( ( toIdx % 8 ) + 'a' ) << ( toIdx / 8 ) + 1 << std::endl;
			}
		}
	return;
	}


/******************************************************
* Minimax Root Call
******************************************************/
static void minimax( Chess::State* root, int depth, Chess::State* bestAction )
	{
	minMaxVal( root, INT_MIN, INT_MAX, depth, MAX, bestAction );
	return;
	}


/******************************************************
* MaxVal
* Returns the maximum value of the passed state's children
* If the passed state pointer is non-null, it will also
* return a pointer to the maximum valued state
******************************************************/
static int minMaxVal( Chess::State* state, int alpha, int beta, int depth, MinMax m, Chess::State* returnAction )
	{
	// Check depth limit
	if( depth == 0 )
		{
		return state->score;
		}

	// Declarations
	std::vector<Chess::State*>	frontier;
	int							val;
	int							bestVal = ( m == MIN ? INT_MAX : INT_MIN );
	Chess::State*				bestAction;
	std::vector<Chess::State*>::iterator 
								runner;

	// Build frontier
	state->Actions( frontier, ( m == MIN ? OPPONENT : ME ) );
	expanded++;

	if( frontier.size() == 0 )
		{
		return( m == MIN ? INT_MAX : INT_MIN );
		}		

	// Read history table and sort accordingly
	for( runner = frontier.begin(); runner != frontier.end(); runner++ )
		{
		if( historyTable.find( *runner ) == historyTable.end() )
			{
			( *runner )->setHistoryVal( 0 );
			}
		else
			{
			( *runner )->setHistoryVal( historyTable[ *runner ] );
			}
		}
	std::sort( frontier.begin(), frontier.end(), StateSort() );

	// Evaluate each state in frontier
	bestAction = frontier.front();
	if( m == MIN )
		{
		for( runner = frontier.begin(); runner != frontier.end() && clock() < endTime; runner++ )
			{
			val = minMaxVal( *runner, alpha, beta, depth - 1, MAX, nullptr );
			( *runner )->score = val;

			// Update values if better state found
			if( val < bestVal )
				{
				bestVal = val;
				bestAction = ( *runner );
				}
			beta = MIN( val, beta );

			// Prune if fail-low
			if( val <= alpha )
				{
				pruned++;
				break;
				}
			}
		}

	else // ( m == MAX )
		{
		for( runner = frontier.begin(); runner != frontier.end() && clock() < endTime; runner++ )
			{
			val = minMaxVal( *runner, alpha, beta, depth - 1, MIN, nullptr );

			// Update values if better state found
			if( val > bestVal )
				{
				bestVal = val;
				bestAction = ( *runner );
				}
			alpha = MAX( val, alpha );

			// Prune if fail-high
			if( val >= beta )
				{
				pruned++;
				break;
				}
			}
		}

	// Update history table
	if( historyTable.find( *bestAction ) == historyTable.end() )
		{
		if( historyTable.size() >= histTableMaxSz )
			{
			historyTable.erase( historyTable.begin() );
			}
		historyTable[ bestAction ] = 0;
		}
	else
		{
		historyTable[ *bestAction ] += 1;
		}

	// Free memory (and return if root call)
	for( int i = 0; i < frontier.size(); i++ )
		{
		if( returnAction != nullptr && frontier[ i ] == bestAction )
			{
			*returnAction = *bestAction;
			}
		else
			{
			delete frontier[ i ];
			}
		}

	// Return value
	return bestVal;
	}
