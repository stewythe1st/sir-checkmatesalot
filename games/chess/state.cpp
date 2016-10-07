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


/******************************************************
* Macros
******************************************************/
#define inSameRow( x, y )	( ( x / 8 ) == ( y / 8 ) )
#define isValidIdx( x )		( ( x <= 63 ) && ( x >= 0 ) )


/******************************************************
* Local Functions
******************************************************/
static Bitboard makeDiffBitboard(int idx1, int idx2);


/******************************************************
* State Constructor
* constructs an initial state from the passed game 
* and AI objects
******************************************************/
Chess::State::State( Chess::Game* game, Chess::AI* ai )
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
	Bitboard diffBitboard;
	int idx;
	int i;

	/**************************************************
	* Pawn Move Validation
	**************************************************/
	int dir = 1;
	if( color == BLACK )
		dir = -1;
	temp = 0xFFFFFFFFFFFFFFFF;
	while( ( idx = bitScanForward( myPawns & temp ) ) != -1 )
		{
		temp.reset( idx );
		if( !isValidIdx( idx + ( 2 * 8 * dir ) ) )
			{
			continue; //add stuff here later
			}
		else
			{
			if( ( ( idx + ( 7 * dir ) ) / 8 == ( ( idx / 8 ) + dir ) ) && allOpp.test( idx + ( 7 * dir ) ) && !allMy.test( idx + ( 7 * dir ) ) )
				moves.emplace_back( &myPawns, makeDiffBitboard( idx, idx + 7 * dir ) );
			if( !all.test( idx + ( 8 * dir ) ) )
				moves.emplace_back( &myPawns, makeDiffBitboard( idx, idx + 8 * dir ) );
			if( ( ( color == WHITE && idx / 8 == 1 ) || ( color == BLACK && idx / 8 == 6 ) ) && ( !all.test( idx + ( 16 * dir ) ) ) && ( !all.test( idx + ( 8 * dir ) ) ) )
				moves.emplace_back( &myPawns, makeDiffBitboard( idx, idx + 16 * dir ) );
			if( ( ( idx + ( 9 * dir ) ) / 8 == ( ( idx / 8 ) + dir ) ) && allOpp.test( idx + ( 9 * dir ) ) && !allMy.test( idx + ( 9 * dir ) ) )
				moves.emplace_back( &myPawns, makeDiffBitboard( idx, idx + 9 * dir ) );
			}
		}
	/**************************************************
	* Rook Move Validation
	**************************************************/
	temp = 0xFFFFFFFFFFFFFFFF;
	i = 0;
	while( ( idx = bitScanForward( myRooks & temp ) ) != -1 )
		{
		temp.reset( idx );
		for( i = idx + 8; ( isValidIdx( i ) && !all.test( i ) ); i += 8 )
			moves.emplace_back( &myPawns, makeDiffBitboard( idx, i ) );
		for( i = idx - 8; ( isValidIdx( i ) && !all.test( i ) ); i -= 8 )
			moves.emplace_back( &myPawns, makeDiffBitboard( idx, i ) );
		for( i = idx - 1; ( isValidIdx( i ) && !all.test( i ) ) && inSameRow( idx, i ); i -= 1 )
			moves.emplace_back( &myPawns, makeDiffBitboard( idx, i ) );
		for( i = idx - 1; ( isValidIdx( i ) && !all.test( i ) ) && inSameRow( idx, i ); i -= 1 )
			moves.emplace_back( &myPawns, makeDiffBitboard( idx, i ) );
		}
	return;
	}


/******************************************************
* Forward Bit Scan
* Returns the index of the first set bit
* Source: https://chessprogramming.wikispaces.com/Bitscan
******************************************************/
const int index64[ 64 ] = {
	0,  1, 48,  2, 57, 49, 28,  3,
	61, 58, 50, 42, 38, 29, 17,  4,
	62, 55, 59, 36, 53, 51, 43, 22,
	45, 39, 33, 30, 24, 18, 12,  5,
	63, 47, 56, 27, 60, 41, 37, 16,
	54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10,
	25, 14, 19,  9, 13,  8,  7,  6
	};
int bitScanForward( Bitboard bb )
	{
	if( bb == 0 )
		return -1;
	unsigned long long bb_ull = bb.to_ullong();
	const unsigned long long debruijn64 = 0x03f79d71b4cb0a89;
	return index64[ ( ( bb_ull & -bb_ull ) * debruijn64 ) >> 58 ];
	}


/******************************************************
* Make Differential Bitboard
* Returns a bitboard with the passed indices set to 1
* Used in constructing differential bitboards
******************************************************/
static Bitboard makeDiffBitboard( int x, int y )
	{
	Bitboard diff = 0;
	diff.set( x );
	diff.set( y );
	return diff;
	}