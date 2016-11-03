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
#include <time.h>


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
static clock_t		endTime;


/******************************************************
* Return Statistics
******************************************************/
void getStats( int& p, int& e )
	{
	p = pruned;
	e = expanded;
	return;
	}


/******************************************************
* Iterative Deepening Minimax Root Call
* time is given in ns
******************************************************/
void id_minimax( Chess::State* root, Chess::State* bestAction, int maxDepth, double time )
	{
	//endTime = clock() + ( time * 0.03 / 1000000000 * CLOCKS_PER_SEC );
	endTime = clock() + ( 5 * CLOCKS_PER_SEC );
	pruned = 0;
	expanded = 0;
	for( int i = 1; i <= maxDepth; i++ )
		{
		minimax( root, i, bestAction );
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
static int minMaxVal( Chess::State* state, int alpha, int beta, int depth, MinMax m, Chess::State* bestAction )
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

	// Build frontier
	state->Actions( frontier, ( m == MIN ? OPPONENT : ME ) );
	expanded++;

	// Evaluate each state in frontier
	std::vector<Chess::State*>::iterator runner = frontier.begin();

	if( m == MIN )
		{
		for( runner; runner != frontier.end() && clock() < endTime; runner++ )
			{
			val = minMaxVal( *runner, alpha, beta, depth - 1, MAX, nullptr );
			( *runner )->score = val;

			// Update values if better state found
			bestVal = MIN( val, bestVal );
			beta = MIN( val, beta );

			// Prune if fail-high
			if( alpha >= beta )
				{
				pruned++;
				break;
				}
			}
		}

	else // ( m == MAX )
		{
		for( runner; runner != frontier.end() && clock() < endTime; runner++ )
			{
			val = minMaxVal( *runner, alpha, beta, depth - 1, MIN, nullptr );
			( *runner )->score = val;

			// Update values if better state found
			bestVal = MAX( val, bestVal );
			alpha = MAX( val, alpha );

			// Prune if fail-low
			if( beta <= alpha )
				{
				pruned++;
				break;
				}
			}
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
			{
			delete frontier[ i ];
			}
		}

	// Return value
	return bestVal;
	}
