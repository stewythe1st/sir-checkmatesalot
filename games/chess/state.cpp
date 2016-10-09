/**************************************************************
* state.cpp
* Definitions for the chess AI state class
* CS5400, FS 2016
* Stuart Miller
**************************************************************/


/******************************************************
* Includes
******************************************************/
#include "ai.h"
#include "state.h"
#include "player.h"
#include "game.h"
#include "piece.h"
#include <map>
#include <cmath>

#define DEBUG_PRINT		( true )
#define NOT_THREATENED	( -1 )

/******************************************************
* Macros
******************************************************/
#define inSameRow( x, y )		( ( ( x ) / 8 ) == ( ( y ) / 8 ) )
#define rankDiff( x, y )		( abs( ( ( x ) / 8 ) - ( ( y ) / 8 ) ) )
#define isValidIdx( x )			( ( ( x ) <= 63 ) && ( ( x ) >= 0 ) )
#define oneRowCross( x, y )		( std::abs( ( ( x ) % 8 ) - ( ( y ) % 8 ) ) == 7 )
#define twoRowCross( x, y )		( std::abs( ( ( x ) % 8 ) - ( ( y ) % 8 ) ) > 5 )
#define getRankNum( x )			( ( x ) / 8 )
#define getFileNum( x )			( ( x ) % 8 )
#define testIdx( bb, idx )		( isValidIdx( idx ) ? bb.test( idx ) : false )


/******************************************************
* Local Variables
******************************************************/
static const int index64[ 64 ] = {
	0,  1, 48,  2, 57, 49, 28,  3,
	61, 58, 50, 42, 38, 29, 17,  4,
	62, 55, 59, 36, 53, 51, 43, 22,
	45, 39, 33, 30, 24, 18, 12,  5,
	63, 47, 56, 27, 60, 41, 37, 16,
	54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10,
	25, 14, 19,  9, 13,  8,  7,  6
	};
static const int knightMoves[ 8 ]	= { -17, -15, -10, -6, 6, 10, 15, 17 };
const int kingMoves[ 8 ]		= {  -9,  -8,  -7, -1, 1,  7,  8,  9 };

/******************************************************
* State Constructor
* constructs an initial state from the passed game 
* and AI objects
******************************************************/
Chess::State::State( Chess::AI* ai )
	{
	// conversion map
	std::map<std::string, Bitboard*> pieceConvert;
	pieceConvert[ "myPawn" ]	= &myPawns;
	pieceConvert[ "myRook" ]	= &myRooks;
	pieceConvert[ "myKnight" ]	= &myKnights;
	pieceConvert[ "myBishop" ]	= &myBishops;
	pieceConvert[ "myQueen" ]	= &myQueens;
	pieceConvert[ "myKing" ]	= &myKing;
	pieceConvert[ "oppPawn" ]	= &oppPawns;
	pieceConvert[ "oppRook" ]	= &oppRooks;
	pieceConvert[ "oppKnight" ] = &oppKnights;
	pieceConvert[ "oppBishop" ] = &oppBishops;
	pieceConvert[ "oppQueen" ]	= &oppQueens;
	pieceConvert[ "oppKing" ]	= &oppKing;

	// Initialize all bitboards to zero
	myPawns		= 0;
	myRooks		= 0;
	myKnights	= 0;
	myBishops	= 0;
	myQueens	= 0;
	myKing		= 0;
	oppPawns	= 0;
	oppRooks	= 0;
	oppKnights	= 0;
	oppBishops	= 0;
	oppQueens	= 0;
	oppKing		= 0;

	std::vector<Chess::Piece*>::iterator piece;
	std::vector<Chess::Piece*>::iterator end;

	// Read in our color
	color = BLACK;
	if( ai->player->color == "White" )
		{
		color = WHITE;
		}

	// Check if we're in check
	inCheck = false;
	if( ai->player->inCheck )
		{
		inCheck = true;
		}		

	// If Opp's last move was a double pawn step, we can en passant capture it
	en_passant = -1;
	if( ai->game->moves.size() > 0 )
		{
		Chess::Move* oppMove = ai->game->moves.back();
		if( oppMove->piece->type == "Pawn" && ( std::abs( oppMove->fromRank - oppMove->toRank ) ) == 2 )
			en_passant = getBitboardIdx( oppMove->toRank, &oppMove->toFile );
		}

	// Read in our pieces
	piece = ai->player->pieces.begin();
	end = ai->player->pieces.end();
	for( piece; piece != end; piece++ )
		{
		( pieceConvert[ ( "my" + ( *piece )->type ) ] )->set( getBitboardIdx( ( *piece )->rank, &( *piece )->file ) );
		}

	// Read in our opponent's pieces
	piece = ai->player->opponent->pieces.begin();
	end = ai->player->opponent->pieces.end();
	for( piece; piece != end; piece++ )
		{
		( pieceConvert[ ( "opp" + ( *piece )->type ) ] )->set( getBitboardIdx( ( *piece )->rank, &( *piece )->file ) );
		}
	
	// Check for state repetition
	invalid_from_idx = -1;
	invalid_to_idx = -1;
	std::vector<Chess::Move*> moves = ai->game->moves;
	int moveSz = moves.size();
	if( moveSz >= 8 )
		{
		bool repetition = true;
		for( int i = 1; i <= 3; i++ )
			{
			if( moves[ moveSz - i ]->fromFile != moves[ moveSz - i - 4 ]->fromFile
			 || moves[ moveSz - i ]->toFile != moves[ moveSz - i - 4 ]->toFile
			 || moves[ moveSz - i ]->fromRank != moves[ moveSz - i - 4 ]->fromRank
			 || moves[ moveSz - i ]->toRank != moves[ moveSz - i - 4 ]->toRank )
				{
				repetition = false;
				break;
				}
			}
		if( repetition )
			{
			std::cout << "Warning! Risk of repetition!" << std::endl;
			invalid_from_idx = getBitboardIdx( moves[ moveSz - 4 ]->fromRank, &moves[ moveSz - 4 ]->fromFile );
			invalid_to_idx = getBitboardIdx( moves[ moveSz - 4 ]->toRank, &moves[ moveSz - 4 ]->toFile );
			}
		}
	return;
	}


/******************************************************
* Get Bitboard Index
* Converts rank and file to a bitboard index
******************************************************/
int getBitboardIdx( int rank, std::string* file )
	{
	return( ( ( *file )[ 0 ] - 'a' ) + ( ( rank - 1 ) * 8 ) );
	}


/******************************************************
* Print Bitboard
* Formats and prints a bitobard to the console
******************************************************/
void print_bitboard( Bitboard* bitboard )
	{
	std::string str = "Val: " + bitboard->to_string() + "\n";
	for( int i = 7; i >= 0; i-- )
		{
		for( int j = 0; j <= 7; j++ )
			{
			if( ( *bitboard ).test( j + ( i * 8 ) ) )
				{
				str += "O";
				}
			else
				{
				str += "+";
				}
			}
		str += "\n";
		}
	str += "\n";
	std::cout << str;
	}

/******************************************************
* Actions Function
* Generates all possible moves from the current state
******************************************************/
void Chess::State::Actions( std::vector<Chess::CondensedMove>& moves )
	{
	/**************************************************
	* Declarations
	**************************************************/
	Bitboard allMy	= myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
	Bitboard allOpp = oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
	Bitboard all	= allMy | allOpp;
	Bitboard temp;
	Bitboard pieces;
	Bitboard diffBitboard;
	int idx, new_idx;
	int i;

	/**************************************************
	* Look for moves to escape check
	**************************************************/
	/*if( inCheck )
		{
		std::cout << "We're in check" << std::endl;
		idx = bitScanForward( myKing );

		// Can king move out to escape?
		for( i = 0; i < 8; i++ )
			{
			if( isValidIdx( idx + kingMoves[ i ] )
				&& !allMy.test( idx + kingMoves[ i ] )
				&& !oneRowCross( idx, idx + kingMoves[ i ] ) )
				{
				if( isThreatened( idx, idx + kingMoves[ i ] ) == -1 )
					addMove( moves, idx, idx + kingMoves[ i ], false );
				}
			}

		// Can we block the attacking piece?


		return;
		}*/

	/**************************************************
	* Pawn Move Validation
	**************************************************/
	int dir = 1;
	if( color == BLACK )
		dir = -1;
	pieces = myPawns;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		new_idx = idx + ( 7 * dir );
		if( allOpp.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			addMove( moves, idx, idx + 7 * dir, &myPawns );
		new_idx = idx + ( 8 * dir );
		if( !all.test( new_idx ) )
			addMove( moves, idx, idx + 8 * dir, &myPawns );
		new_idx = idx + ( 9 * dir );
		if( allOpp.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			addMove( moves, idx, idx + 9 * dir, &myPawns );
		}
		if( ( color == WHITE && getRankNum( idx ) == 1 ) || ( color == BLACK && getRankNum( idx ) == 6 ) )
			{
			if( !all.test( idx + ( 16 * dir ) ) && !all.test( idx + ( 8 * dir ) ) )
				addMove( moves, idx, idx + 16 * dir, &myPawns );
			}
		if( en_passant != -1 )
			{
			if( color == WHITE )
				{
				if( ( ( idx + 1 ) == en_passant ) && !all.test( idx + 9 ) )
					addMove( moves, idx, idx + 9, &myPawns );
				if( ( ( idx - 1 ) == en_passant ) && !all.test( idx + 7 ) )
					addMove( moves, idx, idx + 7, &myPawns );
				}
			else
				{
				if( ( ( idx + 1 ) == en_passant ) && !all.test( idx - 7 ) )
					addMove( moves, idx, idx - 7, &myPawns );
				if( ( ( idx - 1 ) == en_passant ) && !all.test( idx - 9 ) )
					addMove( moves, idx, idx - 9, &myPawns );
				}
			}
			

	/**************************************************
	* Rook Move Validation
	**************************************************/
	pieces = myRooks;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		for( i = idx + 8; ( isValidIdx( i ) && !all.test( i ) ); i += 8 )
			addMove( moves, idx, i, &myRooks );
		for( i = idx - 8; ( isValidIdx( i ) && !all.test( i ) ); i -= 8 )
			addMove( moves, idx, i, &myRooks );
		for( i = idx + 1; ( isValidIdx( i ) && !all.test( i ) ) && inSameRow( idx, i ); i += 1 )
			addMove( moves, idx, i, &myRooks );
		for( i = idx - 1; ( isValidIdx( i ) && !all.test( i ) ) && inSameRow( idx, i ); i -= 1 )
			addMove( moves, idx, i, &myRooks );
		}

	/**************************************************
	* Bishop Move Validation
	**************************************************/
	pieces = myBishops;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
	{
		pieces.reset( idx );
		for( i = idx + 9; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 0 ) ); i += 9 )
			addMove( moves, idx, i, &myBishops );
		for( i = idx - 9; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 7 ) ); i -= 9 )
			addMove( moves, idx, i, &myBishops );
		for( i = idx + 7; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 7 ) ); i += 7 )
			addMove( moves, idx, i, &myBishops );
		for( i = idx - 7; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 0 ) ); i -= 7 )
			addMove( moves, idx, i, &myBishops );
	}

	/**************************************************
	* Queen Move Validation
	**************************************************/
	pieces = myQueens;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		for( i = idx + 8; ( isValidIdx( i ) && !all.test( i ) ); i += 8 )
			addMove( moves, idx, i, &myQueens );
		for( i = idx - 8; ( isValidIdx( i ) && !all.test( i ) ); i -= 8 )
			addMove( moves, idx, i, &myQueens );
		for( i = idx + 1; ( isValidIdx( i ) && !all.test( i ) ) && inSameRow( idx, i ); i += 1 )
			addMove( moves, idx, i, &myQueens );
		for( i = idx - 1; ( isValidIdx( i ) && !all.test( i ) ) && inSameRow( idx, i ); i -= 1 )
			addMove( moves, idx, i, &myQueens );
		for( i = idx + 9; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 0 ) ); i += 9 )
			addMove( moves, idx, i, &myQueens );
		for( i = idx - 9; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 7 ) ); i -= 9 )
			addMove( moves, idx, i, &myQueens );
		for( i = idx + 7; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 7 ) ); i += 7 )
			addMove( moves, idx, i, &myQueens );
		for( i = idx - 7; ( isValidIdx( i ) && !all.test( i ) && ( getFileNum( i ) != 0 ) ); i -= 7 )
			addMove( moves, idx, i, &myQueens );
		}

	/**************************************************
	* Knight Move Validation
	**************************************************/
	pieces = myKnights;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		for( i = 0; i < 8; i++ )
			{
			new_idx = idx + knightMoves[ i ];
			if( isValidIdx( new_idx ) && !allMy.test( new_idx ) && !twoRowCross( idx, new_idx ) )
				addMove( moves, idx, new_idx, &myKnights );
			}
		}

	/**************************************************
	* King Move Validation
	**************************************************/
	idx = bitScanForward( myKing );
	for( i = 0; i < 8; i++ )
		{
		new_idx = idx + kingMoves[ i ];
		if( isValidIdx( new_idx ) && !allMy.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			addMove( moves, idx, new_idx, &myKing );
		}

	return;
	}


/******************************************************
* Forward Bit Scan
* Returns the index of the first set bit
* Source: https://chessprogramming.wikispaces.com/Bitscan
******************************************************/
int bitScanForward( Bitboard bb )
	{
	if( bb == 0 )
		return -1;
	unsigned long long bb_ull = bb.to_ullong();
	const unsigned long long debruijn64 = 0x03f79d71b4cb0a89;
	return index64[ ( ( bb_ull & -bb_ull ) * debruijn64 ) >> 58 ];
	}


//int Chess::State::isThreatened( int idx ) { return this->isThreatened( idx, idx ); }
int Chess::State::isThreatened( int idx, int to_idx, int from_idx )
	{
	if( DEBUG_PRINT ) std::cout << "Testing for threats to idx: " << idx << std::endl;
	Bitboard allMy = myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
	allMy.reset( from_idx );
	allMy.set( to_idx );
	Bitboard allOpp = oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
	allOpp.reset( to_idx );
	Bitboard all = allMy | allOpp;
	all.reset( idx );
	Bitboard pieces;
	int i;
	int dir = 1;
	if( color == BLACK )
		dir = -1;

	// Check for attacking pawns
	if( DEBUG_PRINT ) std::cout << " Looking for pawns" << std::endl;
	pieces = oppPawns;
	pieces.reset( to_idx );
	if( isValidIdx( idx + ( 7 * dir ) ) && pieces.test( idx + ( 7 * dir ) ) && !oneRowCross( idx, idx + ( 7 * dir ) ) )
		return idx + ( 7 * dir );
	if( isValidIdx( idx + ( 9 * dir ) ) && pieces.test( idx + ( 9 * dir ) ) && !oneRowCross( idx, idx + ( 9 * dir ) ) )
		return idx + ( 9 * dir );

	// Check for attacking bishops or queens (diagonally)
	if( DEBUG_PRINT ) std::cout << " Looking for bishops" << std::endl;
	pieces = oppBishops | oppQueens;
	pieces.reset( to_idx );
	for( i = idx; ( isValidIdx( i + 9 ) && !testIdx( all, i ) && ( getFileNum( i ) != 7 ) ); i += 9 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 9 ) && !testIdx( all, i ) && ( getFileNum( i ) != 0 ) ); i -= 9 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i + 7 ) && !testIdx( all, i ) && ( getFileNum( i ) != 0 ) ); i += 7 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 7 ) && !testIdx( all, i ) && ( getFileNum( i ) != 7 ) ); i -= 7 ) {};
	if( pieces.test( i ) ) return i;

	// Check for attacking rooks or queens (obliques)
	if( DEBUG_PRINT ) std::cout << " Looking for rooks" << std::endl;
	pieces = oppRooks | oppQueens;
	pieces.reset( to_idx );
	for( i = idx; ( isValidIdx( i + 8 ) && !all.test( i ) ); i += 8 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 8 ) && !all.test( i ) ); i -= 8 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i + 1 ) && !all.test( i ) ) && inSameRow( idx, i ); i += 1 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 1 ) && !all.test( i ) ) && inSameRow( idx, i ); i -= 1 ) {};
	if( pieces.test( i ) ) return i;

	// Check for attacking knights
	if( DEBUG_PRINT ) std::cout << " Looking for knights" << std::endl;
	pieces = oppKnights;
	pieces.reset( to_idx );
	int new_idx;
	for( i = 0; i < 8; i++ )
		{
		new_idx = idx + knightMoves[ i ];
		if( isValidIdx( new_idx ) && pieces.test( new_idx ) && !twoRowCross( idx, new_idx ) )
			return new_idx;
		}

	// Check for attacking kings (yes, I guess that is a thing...)
	if( DEBUG_PRINT ) std::cout << " Looking for kings" << std::endl;
	pieces = oppKing;
	pieces.reset( to_idx );
	for( i = 0; i < 8; i++ )
		{
		new_idx = idx + kingMoves[ i ];
		if( isValidIdx( new_idx ) && pieces.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			return new_idx;
		}

	if( DEBUG_PRINT ) std::cout << " We're safe at idx: " << idx << std::endl;
	return NOT_THREATENED;
	}

void Chess::State::addMove( std::vector<Chess::CondensedMove>& moves, int from_idx, int to_idx, Bitboard* piece )
	{
	std::cout << "Testing move from " << from_idx << " to " << to_idx << ":   ";
	Bitboard diff = 0;
	diff.set( from_idx );
	diff.set( to_idx );

	// see if this moves would cause repetition
	if( from_idx == invalid_from_idx && to_idx == invalid_to_idx )
		{
		std::cout << "Would cause repetition!" << std::endl;
		return;
		}

	// apply move and see if the king is still/newly in check
	Bitboard temp = *piece;
	//*piece ^= diff; // apply move
	piece->set( to_idx );
	piece->reset( from_idx );
	int kingIdx = ( ( piece == &myKing ) ? bitScanForward( *piece ) : bitScanForward( myKing ) );
	if( isThreatened( kingIdx, to_idx, from_idx ) != NOT_THREATENED )
		{
		std::cout << "Puts King in check!" << std::endl;
		piece->reset( to_idx );
		piece->set( from_idx );
		return;
		}
	//*piece = temp; // revert move
	piece->reset( to_idx );
	piece->set( from_idx );
	
	// if we made it this far, the move is valid
	std::cout << "Is valid!" << std::endl;
	moves.emplace_back( piece, diff );

	return;
	}