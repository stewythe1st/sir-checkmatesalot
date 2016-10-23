/**************************************************************
* ai.cpp
* Definitions for the chess base AI class
* CS5400, FS 2016
* Stuart Miller
**************************************************************/


/**************************************************************
* Includes
**************************************************************/
#include "ai.h"
#include "minimax.h"
#include <algorithm>


/**************************************************************
* Name Accessor
* Returns team neame to the game server
**************************************************************/
std::string Chess::AI::getName() { return "Sir Checkmatesalot"; }


/**************************************************************
* Start
* This is automatically called when the game first starts, once
* the Game object and all GameObjects have been initialized,
* but before any players do anything. This is a good place to
* initialize any variables you add to your AI, or start tracking
* game objects.
**************************************************************/
void Chess::AI::start() {}


/**************************************************************
* Game Updated
* This is automatically called every time the game (or anything
* in it) updates. If a function you call triggers an update this
* will be called before that function returns.
**************************************************************/
void Chess::AI::gameUpdated() {}


/**************************************************************
* Game Ended
* This is automatically called when the game ends. You can do
* any cleanup of you AI here, or do custom logging. After this
* function returns the application will close.
*   param won: true if your player won, false otherwise
*   param reason: a string explaining why you won or lost
**************************************************************/
void Chess::AI::ended( bool won, std::string reason ) 
	{
	printBoard();
	}


/**************************************************************
* Run Turn
* This is called every time it is this AI.player's turn.
*     return: Represents if you want to end your turn. True
*             means end your turn, False means to keep your
*             turn going and re-call this function.
**************************************************************/
bool Chess::AI::runTurn()
	{
	std::cout << "-------------------------------" << std::endl;
	std::cout << "Beginning turn " << this->game->currentTurn << " for " << this->game->currentPlayer->color << std::endl;

	// Print board to console
	printBoard();

    // Print the Opp's last move to the console
	if( this->game->moves.size() > 0 )
		{
		std::cout << "Opponent's Last Move: '" << this->game->moves[ this->game->moves.size() - 1 ]->san << "'" << std::endl;
		}

    // Print how much time remaining this AI has to calculate moves
    std::cout << "Time Remaining: " << this->player->timeRemaining << " ns" << std::endl;

	// Build initial state for this move
	Chess::State initial( this );
	Chess::State bestAction;

	// Call minimax
	minimax( &initial, &bestAction, 3 );

	// Make our chosen move
	executeMove( &bestAction );

    return true;
}


/**************************************************************
* Execute Move
* Performs the logic to translate potential moves and send
* the chosen move to the game server
**************************************************************/
bool Chess::AI::executeMove( Chess::State* move )
	{
	int idx;
	int toIdx = ( move->misc.to_ullong() & TOIDX_MASK ) >> TOIDX_BITSHIFT;
	int fromIdx = ( move->misc.to_ullong() & FROMIDX_MASK ) >> FROMIDX_BITSHIFT;
	std::vector<Chess::Piece*>::iterator runner = this->player->pieces.begin();
	std::vector<Chess::Piece*>::iterator last = this->player->pieces.end();
	for( runner; runner != last; runner++ )
		{
		idx = getBitboardIdx( ( *runner )->rank, &( *runner )->file );
		if( idx == fromIdx )
			{
			std::string toFile( 1, ( char )( toIdx % 8 ) + 'a' );
			int toRank = 1 + toIdx / 8;
			std::cout << "Moving " << ( *runner )->type << " at "
					  << ( *runner )->file << ( *runner )->rank
					  << " to " << toFile << toRank << std::endl;
			return ( *runner )->move( toFile, toRank, "Queen" );
			}
		}
	return false;
	}


/**************************************************************
* Print Board
* Outputs chess board to the console in a user-readable format
**************************************************************/
void Chess::AI::printBoard()
	{
	for( int rank = 9; rank >= -1; rank-- )
		{
		std::string str = "";
		if( rank == 9 || rank == 0 ) // then the top or bottom of the board
			{
			str = "   +------------------------+";
			}
		else if( rank == -1 ) // then show the ranks
			{
			str = "     a  b  c  d  e  f  g  h";
			}
		else // board
			{
			str += " ";
			str += std::to_string( rank );
			str += " |";
			// fill in all the files with pieces at the current rank
			for( int fileOffset = 0; fileOffset < 8; fileOffset++ )
				{
				std::string file( 1, ( char )( ( ( int )"a"[ 0 ] ) + fileOffset ) ); // start at a, with with file offset increasing the char;
				Chess::Piece* currentPiece = nullptr;
				for( auto piece : this->game->pieces )
					{
					if( piece->file == file && piece->rank == rank ) // then we found the piece at (file, rank)
						{
						currentPiece = piece;
						break;
						}
					}

				char code = '.'; // default "no piece";
				if( currentPiece != nullptr )
					{
					code = currentPiece->type[ 0 ];

					if( currentPiece->type == "Knight" ) // 'K' is for "King", we use 'N' for "Knights"
						{
						code = 'N';
						}

					if( currentPiece->owner->id == "1" ) // the second player (black) is lower case. Otherwise it's upppercase already
						{
						code = tolower( code );
						}
					}

				str += " ";
				str += code;
				str += " ";
				}

			str += "|";
			}

		std::cout << str << std::endl;
		}
	}
