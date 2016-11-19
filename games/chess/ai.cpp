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
#include "globals.h"
#include "fathom/tbprobe.h"
#include "fathom/tbaccess.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <unistd.h>


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
void Chess::AI::start()
	{
		
	// Print startup text
	std::cout << "       ___ _" << std::endl
			  << "      / __\\ |___   ___  ___ ___  " << std::endl
			  << "     / /  | '_  \\ / _ \\/ __/ __| " << std::endl
			  << "    / /___| | | ||  __/\\__ \\__ \\ " << std::endl
			  << "    \\____/|_| |_| \\___||___/___/ " << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << this->game->currentPlayer->name << " vs. " << this->game->currentPlayer->opponent->name << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	
	// Read in cfg values
	std::ifstream fileIn;
	std::string line, name, value;
	std::cout << "Reading in Configuration:" << std::endl;
	fileIn.open( "games/chess/conf/chess.cfg" );
	while( std::getline( fileIn, line ) )
		{

		// Strip comments
		line = line.substr( 0, line.find( '#' ) );

		// Strip whitespace
		line.erase( std::remove( line.begin(), line.end(), ' ' ), line.end() );
		line.erase( std::remove( line.begin(), line.end(), '\n' ), line.end() );
		line.erase( std::remove( line.begin(), line.end(), '\r' ), line.end() );
		line.erase( std::remove( line.begin(), line.end(), '\t' ), line.end() );

		if( line.length() > 0 )
			{
			// Read parameter name
			name = line.substr( 0, line.find( "=" ) );
			std::transform( name.begin(), name.end(), name.begin(), ::tolower );

			// Read value
			value = line.substr( line.find( "=" ) + 1 );

			setGlobal( name, value );
			}
		}	
	fileIn.close();

	return;
	}


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
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "Beginning turn " << this->game->currentTurn << " for " << this->game->currentPlayer->color << "(" << this->game->currentPlayer->name << ")" << std::endl;

	// Print board to console
	printBoard();

	std::cout << "State:" << std::endl;

    // Print the Opp's last move to the console
	if( this->game->moves.size() > 0 )
		{
		std::cout << "  Opponent's Last Move: '" << this->game->moves[ this->game->moves.size() - 1 ]->san << "'" << std::endl;
		}

	// Print board state in Forsyth-Edwards notation
	std::cout << "  FEN: " << this->game->fen << std::endl;

    // Print how much time remaining this AI has to calculate moves
	int start_ms = this->player->timeRemaining / 1000000;

    std::cout << "  Time Remaining: " 
			  <<  start_ms / 60000  << ":"
			  << std::setw(2) << std::setfill( '0' ) << ( start_ms / 1000 ) % 60 << "." 
			  << std::setw(3) << start_ms % 1000 << "s" << std::endl;

	// Build initial state for this move
	Chess::State initial( this );
	Chess::State bestAction;
	bool err = false;
	bool endGame = false;

	if( useEndGameTables )
		{
		// Check to see if we're in endgame
		endGame = this->game->pieces.size() <= 5;			// Need better position checking than this
		if( endGame ) 
			{

			// Probe endgame table
			Chess::State tablebaseMove = initial;
			err = probeTablebases( &tablebaseMove );		// Still sometimes fails unexpectantly, or makes a dumb move (idk why...)

			// Execute move, or fallback to minimax
			if( err )
				{
				std::cout << "  Falling back to minimax" << std::endl;
				}
			else
				{
				executeMove( &tablebaseMove );
				}
			}
		}

	if( !useEndGameTables || err || !endGame )
		{
		// Call minimax
		std::cout << "Calculating Best Move:" << std::endl;
		id_minimax( &initial, &bestAction, this->player->timeRemaining );

		// Make our chosen move
		executeMove( &bestAction );

		// Print node stats
		int pruned, expanded, expandedNQ, depth;
		getStats( pruned, expanded, expandedNQ, depth );
		std::cout << "Statistics: " << std::endl;
		std::cout << "  Pruned Nodes: " << pruned << std::endl;
		std::cout << "  Expanded Nodes: " << expanded << std::endl;
		std::cout << "  Expanded NonQuiescent Nodes: " << expandedNQ << std::endl;
		int end_ms = start_ms - ( this->player->timeRemaining / 1000000 );
		std::cout << "  Time Spent: " << end_ms / 1000 << "." << end_ms % 1000 << "s" << std::endl;
		std::cout << "  Depth Achieved: " << depth - 1 << std::endl;
		}

	// Done
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "Beginning turn " << this->game->currentTurn + 1 << " for " << this->game->currentPlayer->opponent->color << std::endl;
	std::cout << "  Waiting on Opponent..." << std::endl;
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
		idx = getBitboardIdx( ( *runner )->rank, ( *runner )->file );
		if( idx == fromIdx )
			{
			std::string toFile( 1, ( char )( toIdx % 8 ) + 'a' );
			int toRank = 1 + toIdx / 8;
			std::cout << "  Moving " << ( *runner )->type << " at "
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

/**************************************************************
* Probe Tablebases
* Looks to endgame tablebases for best move.
**************************************************************/
bool Chess::AI::probeTablebases( Chess::State* rtnState )
	{
	std::cout << "Probing endgame tables:" << std::endl;
	unsigned result, altResult;
	bool err = false;

	// Convert FEN string to tablebase pos struct
	pos fathomPos;
	parse_FEN( &fathomPos, this->game->fen.c_str() );

	// Load tablebases
	std::cout << "  Scanning for tablebases: ";
	char filePath[ 1024 ];
	if( getcwd( filePath, sizeof( filePath ) ) != NULL ) // getcwd() only works on linux systems, apparently.
		{												 // If on a windows system, I guess it will just fail
		strcat( filePath, "/games/chess/endgametables" );// and fallback to regular minimax.
		}
	else
		{
		err = true;
		std::cout << " Error!";
		}
	if( !err )
		{
		err = !tb_init( filePath );
		}
	std::cout << std::endl;

	// Probe tablebases for result
	if( !err )
		{
		std::cout << "  Result: ";
		result = tb_probe_root( fathomPos.white, fathomPos.black, fathomPos.kings, fathomPos.queens, fathomPos.rooks, fathomPos.bishops, fathomPos.knights, fathomPos.pawns, fathomPos.rule50, fathomPos.castling, fathomPos.ep, fathomPos.turn, &altResult );
		}
	if( result != TB_RESULT_FAILED )
		{
		std::cout << TB_GET_FROM( result ) << " to " << TB_GET_TO( result ) << std::endl;
		
		// Convert to/from to a chess state so that the move can be executed
		rtnState->misc = 0;
		rtnState->misc |= ( ( ( unsigned long long )TB_GET_FROM( result ) ) << FROMIDX_BITSHIFT );
		rtnState->misc |= ( ( ( unsigned long long )TB_GET_TO( result ) ) << TOIDX_BITSHIFT );
		}
	else
		{
		err = true;
		std::cout << "Failed!" << std::endl;
		std::cout << "  Error: " << ( result == TB_RESULT_FAILED ) << ( result == TB_RESULT_CHECKMATE ) << ( result == TB_RESULT_STALEMATE ) << std::endl;
		}

	return err;
	}