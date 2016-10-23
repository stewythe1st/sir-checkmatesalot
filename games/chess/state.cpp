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
#include "hueristicVal.h"
#include <map>
#include <cmath>

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
* Copy Constructor
* Performs a deep copy of the passed state
******************************************************/
Chess::State::State( Chess::State* parent )
	{
	myPawns = parent->myPawns;
	myRooks = parent->myRooks;
	myKnights = parent->myKnights;
	myBishops = parent->myBishops;
	myQueens = parent->myQueens;
	myKing = parent->myKing;
	oppPawns = parent->oppPawns;
	oppRooks = parent->oppRooks;
	oppKnights = parent->oppKnights;
	oppBishops = parent->oppBishops;
	oppQueens = parent->oppQueens;
	oppKing = parent->oppKing;
	inCheck = parent->inCheck;
	color = parent->color;
	invalid_from_idx = parent->invalid_from_idx;
	invalid_to_idx = parent->invalid_to_idx;
	en_passant = parent->en_passant;
	canCastleA = parent->canCastleA;
	canCastleH = parent->canCastleH;
	misc = 0;
	}


/******************************************************
* Assignment Operator
* Performs a deep copy of the passed state
******************************************************/
Chess::State& Chess::State::operator = ( const Chess::State &rhs )
	{
	myPawns = rhs.myPawns;
	myRooks = rhs.myRooks;
	myKnights = rhs.myKnights;
	myBishops = rhs.myBishops;
	myQueens = rhs.myQueens;
	myKing = rhs.myKing;
	oppPawns = rhs.oppPawns;
	oppRooks = rhs.oppRooks;
	oppKnights = rhs.oppKnights;
	oppBishops = rhs.oppBishops;
	oppQueens = rhs.oppQueens;
	oppKing = rhs.oppKing;
	inCheck = rhs.inCheck;
	color = rhs.color;
	invalid_from_idx = rhs.invalid_from_idx;
	invalid_to_idx = rhs.invalid_to_idx;
	en_passant = rhs.en_passant;
	canCastleA = rhs.canCastleA;
	canCastleH = rhs.canCastleH;
	misc = rhs.misc;
	score = rhs.score;
	}


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
	misc		= 0;

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
	canCastleA = true;
	canCastleH = true;
	for( piece; piece != end; piece++ )
		{
		( pieceConvert[ ( "my" + ( *piece )->type ) ] )->set( getBitboardIdx( ( *piece )->rank, &( *piece )->file ) );
		if( ( *piece )->type == "King" && ( *piece )->hasMoved )
			{
			canCastleA = false;
			canCastleH = false;
			}
		if( ( *piece )->type == "Rook" && ( *piece )->hasMoved )
			{
			if( (*piece)->file != "a")
				canCastleA = false;
			if( ( *piece )->file != "h" )
				canCastleH = false;
			}
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
			if( DEBUG_PRINT ) std::cout << "Warning! Risk of repetition!" << std::endl;
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
void Chess::State::Actions( std::vector<Chess::State*>& frontier, int player )
	{
	/**************************************************
	* Declarations
	**************************************************/
	Bitboard allMy, allOpp, pieces;
	Bitboard *pawns, *rooks, *knights, *bishops, *queens, *king;
	int dir, idx, new_idx, i, pawnRank;
	if( player == ME )
		{
		pawns	= &myPawns;
		rooks	= &myRooks;
		knights = &myKnights;
		bishops = &myBishops;
		queens	= &myQueens;
		king	= &myKing;
		allMy	= myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
		allOpp	= oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
		dir		= ( color == BLACK ? -1 : 1 );
		pawnRank= ( color == BLACK ?  6 : 1 );
		}
	else
		{
		pawns	= &oppPawns;
		rooks	= &oppRooks;
		knights = &oppKnights;
		bishops = &oppBishops;
		queens	= &oppQueens;
		king	= &oppKing;
		allMy	= oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
		allOpp	= myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
		dir		= ( color == WHITE ? -1 : 1 );
		pawnRank= ( color == WHITE ?  6 : 1 );
		}
	Bitboard all = allMy | allOpp;

	/**************************************************
	* Pawn Move Validation
	**************************************************/
	pieces = *pawns;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		new_idx = idx + ( 7 * dir );
		if( isValidIdx( new_idx ) && allOpp.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			addMove( frontier, idx, new_idx, pawns, player );
		new_idx = idx + ( 8 * dir );
		if( isValidIdx( new_idx ) && !all.test( new_idx ) )
			addMove( frontier, idx, new_idx, pawns, player );
		new_idx = idx + ( 9 * dir );
		if( isValidIdx( new_idx ) && allOpp.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			addMove( frontier, idx, new_idx, pawns, player );
		if( getRankNum( idx ) == pawnRank && !all.test( idx + ( 16 * dir ) ) && !all.test( idx + ( 8 * dir ) ) )
			addMove( frontier, idx, idx + ( 16 * dir ), pawns, player );
		if( en_passant != -1 )
			{
			if( color == WHITE )
				{
				if( ( ( idx + 1 ) == en_passant ) && !all.test( idx + 9 ) && !oneRowCross( idx, idx + 9 ) )
					addMove( frontier, idx, idx + 9, pawns, player );
				if( ( ( idx - 1 ) == en_passant ) && !all.test( idx + 7 ) && !oneRowCross( idx, idx + 7 ) )
					addMove( frontier, idx, idx + 7, pawns, player );
				}
			else
				{
				if( ( ( idx + 1 ) == en_passant ) && !all.test( idx - 7 ) && !oneRowCross( idx, idx - 7 ) )
					addMove( frontier, idx, idx - 7, pawns, player );
				if( ( ( idx - 1 ) == en_passant ) && !all.test( idx - 9 ) && !oneRowCross( idx, idx - 9 ) )
					addMove( frontier, idx, idx - 9, pawns, player );
				}
			}
		}
			

	/**************************************************
	* Rook Move Validation
	**************************************************/
	pieces = *rooks;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		for( i = idx + 8; ( isValidIdx( i ) && !allMy.test( i ) ); i += 8 )
			{
			addMove( frontier, idx, i, rooks, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 8; ( isValidIdx( i ) && !allMy.test( i ) ); i -= 8 )
			{
			addMove( frontier, idx, i, rooks, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx + 1; ( isValidIdx( i ) && !allMy.test( i ) ) && inSameRow( idx, i ); i += 1 )
			{
			addMove( frontier, idx, i, rooks, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 1; ( isValidIdx( i ) && !allMy.test( i ) ) && inSameRow( idx, i ); i -= 1 )
			{
			addMove( frontier, idx, i, rooks, player );
			if( allOpp.test( i ) ) break;
			}
		}

	/**************************************************
	* Bishop Move Validation
	**************************************************/
	pieces = *bishops;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		for( i = idx + 9; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 0 ) ); i += 9 )
			{
			addMove( frontier, idx, i, bishops, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 9; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 7 ) ); i -= 9 )
			{
			addMove( frontier, idx, i, bishops, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx + 7; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 7 ) ); i += 7 )
			{
			addMove( frontier, idx, i, bishops, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 7; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 0 ) ); i -= 7 )
			{
			addMove( frontier, idx, i, bishops, player );
			if( allOpp.test( i ) ) break;
			}
		}

	/**************************************************
	* Queen Move Validation
	**************************************************/
	pieces = *queens;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		for( i = idx + 8; ( isValidIdx( i ) && !allMy.test( i ) ); i += 8 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 8; ( isValidIdx( i ) && !allMy.test( i ) ); i -= 8 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx + 1; ( isValidIdx( i ) && !allMy.test( i ) ) && inSameRow( idx, i ); i += 1 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 1; ( isValidIdx( i ) && !allMy.test( i ) ) && inSameRow( idx, i ); i -= 1 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx + 9; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 0 ) ); i += 9 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 9; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 7 ) ); i -= 9 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx + 7; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 7 ) ); i += 7 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		for( i = idx - 7; ( isValidIdx( i ) && !allMy.test( i ) && ( getFileNum( i ) != 0 ) ); i -= 7 )
			{
			addMove( frontier, idx, i, queens, player );
			if( allOpp.test( i ) ) break;
			}
		}

	/**************************************************
	* Knight Move Validation
	**************************************************/
	pieces = *knights;
	while( ( idx = bitScanForward( pieces ) ) != -1 )
		{
		pieces.reset( idx );
		for( i = 0; i < 8; i++ )
			{
			new_idx = idx + knightMoves[ i ];
			if( isValidIdx( new_idx ) && !allMy.test( new_idx ) && !twoRowCross( idx, new_idx ) )
				addMove( frontier, idx, new_idx, knights, player );
			}
		}

	/**************************************************
	* King Move Validation
	**************************************************/
	idx = bitScanForward( *king );
	for( i = 0; i < 8; i++ )
		{
		new_idx = idx + kingMoves[ i ];
		if( isValidIdx( new_idx ) && !allMy.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			addMove( frontier, idx, new_idx, king, player );
		}
	/*
	if( canCastleA && !isThreatened( idx, idx, idx ) )
		{
		for( i = idx - 1; getFileNum( i ) != 7 && all.test( i ); i-- ) {};
		if( getFileNum( i ) == 0 )
			addMove( frontier, idx, i, king );
		}
	if( canCastleH && !isThreatened( idx, idx, idx ) )
		{
		for( i = idx + 1; getFileNum( i ) != 0 && all.test( i ); i++ ) {};
		if( getFileNum( i ) == 7 )
			addMove( frontier, idx, i, king );
		}
	*/
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


/******************************************************
* Test if index is Threatened
* If square is under attack, returns the index of the
* square found to be attacking it
******************************************************/
int Chess::State::isThreatened( int idx, int to_idx, int from_idx, int player )
	{
	Bitboard allMy, allOpp, pieces;
	Bitboard *pawns, *rooks, *knights, *bishops, *queens, *king;
	int dir;
	if( player == ME )
		{
		pawns	= &oppPawns;
		rooks	= &oppRooks;
		knights = &oppKnights;
		bishops = &oppBishops;
		queens	= &oppQueens;
		king	= &oppKing;
		allMy	= myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
		allOpp	= oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
		dir		= ( color == BLACK ? -1 : 1 );
		}
	else
		{
		pawns	= &myPawns;
		rooks	= &myRooks;
		knights = &myKnights;
		bishops = &myBishops;
		queens	= &myQueens;
		king	= &myKing;
		allMy	= oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
		allOpp	= myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
		dir		= ( color == WHITE ? -1 : 1 );
		}
	Bitboard all = allMy | allOpp;
	all.reset( idx );
	allMy.reset( from_idx );
	allMy.set( to_idx );
	allOpp.reset( to_idx );

	int i;

	// Check for attacking pawns
	pieces = *pawns;
	//pieces.reset( to_idx );
	if( isValidIdx( idx + ( 7 * dir ) ) && pieces.test( idx + ( 7 * dir ) ) && !oneRowCross( idx, idx + ( 7 * dir ) ) )
		return idx + ( 7 * dir );
	if( isValidIdx( idx + ( 9 * dir ) ) && pieces.test( idx + ( 9 * dir ) ) && !oneRowCross( idx, idx + ( 9 * dir ) ) )
		return idx + ( 9 * dir );

	// Check for attacking bishops or queens (diagonally)
	pieces = *bishops | *queens;
	//pieces.reset( to_idx );
	for( i = idx; ( isValidIdx( i + 9 ) && !testIdx( all, i ) && ( getFileNum( i ) != 7 ) ); i += 9 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 9 ) && !testIdx( all, i ) && ( getFileNum( i ) != 0 ) ); i -= 9 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i + 7 ) && !testIdx( all, i ) && ( getFileNum( i ) != 0 ) ); i += 7 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 7 ) && !testIdx( all, i ) && ( getFileNum( i ) != 7 ) ); i -= 7 ) {};
	if( pieces.test( i ) ) return i;

	// Check for attacking rooks or queens (obliques)
	pieces = *rooks | *queens;
	//pieces.reset( to_idx );
	for( i = idx; ( isValidIdx( i + 8 ) && !all.test( i ) ); i += 8 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 8 ) && !all.test( i ) ); i -= 8 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i + 1 ) && !all.test( i ) ) && inSameRow( idx, i + 1 ); i += 1 ) {};
	if( pieces.test( i ) ) return i;
	for( i = idx; ( isValidIdx( i - 1 ) && !all.test( i ) ) && inSameRow( idx, i - 1 ); i -= 1 ) {};
	if( pieces.test( i ) ) return i;

	// Check for attacking knights
	pieces = *knights;
	//pieces.reset( to_idx );
	int new_idx;
	for( i = 0; i < 8; i++ )
		{
		new_idx = idx + knightMoves[ i ];
		if( isValidIdx( new_idx ) && pieces.test( new_idx ) && !twoRowCross( idx, new_idx ) )
			return new_idx;
		}

	// Check for attacking kings (yes, I guess that is a thing...)
	pieces = *king;
	//pieces.reset( to_idx );
	for( i = 0; i < 8; i++ )
		{
		new_idx = idx + kingMoves[ i ];
		if( isValidIdx( new_idx ) && pieces.test( new_idx ) && !oneRowCross( idx, new_idx ) )
			return new_idx;
		}

	return NOT_THREATENED;
	}

void Chess::State::addMove( std::vector<Chess::State*>& frontier, int from_idx, int to_idx, Bitboard* piece, int player )
	{
	if( DEBUG_PRINT ) std::cout << "Testing move from " << from_idx << " to " << to_idx << ":   ";

	// Apply move, copy state, revert move
	piece->set( to_idx );
	piece->reset( from_idx );
	State* newState = new State( this );
	piece->reset( to_idx );
	piece->set( from_idx );

	// Set to/from indices in misc data struct
	newState->misc |= ( ( unsigned long long )from_idx << FROMIDX_BITSHIFT );
	newState->misc |= ( ( unsigned long long )to_idx << TOIDX_BITSHIFT );
	
	// My side processing
	if( player == ME )
		{
		// Remove piece that was attacked
		newState->oppPawns.reset( to_idx );
		newState->oppRooks.reset( to_idx );
		newState->oppKnights.reset( to_idx );
		newState->oppBishops.reset( to_idx );
		newState->oppQueens.reset( to_idx );
		newState->oppKing.reset( to_idx );
		// Special case for pawn promotion (always promotes to queen)
		if( ( piece == &myPawns ) && ( to_idx > 55 || to_idx < 8 ) )
			{
			newState->myQueens.set( to_idx );
			newState->myPawns.reset( to_idx );
			}
		}
	// Opponent's side processing
	else
		{
		// Remove piece that was attacked
		newState->myPawns.reset( to_idx );
		newState->myRooks.reset( to_idx );
		newState->myKnights.reset( to_idx );
		newState->myBishops.reset( to_idx );
		newState->myQueens.reset( to_idx );
		newState->myKing.reset( to_idx );
		// Special case for pawn promotion (always promotes to queen)
		if( ( piece == &oppPawns ) && ( to_idx > 55 || to_idx < 8 ) )
			{
			newState->oppQueens.set( to_idx );
			newState->oppPawns.reset( to_idx );
			}
		}

	// see if this move would cause repetition
	/*if( from_idx == invalid_from_idx && to_idx == invalid_to_idx )
		{
		if( DEBUG_PRINT ) std::cout << "Would cause repetition!" << std::endl;
		return;
		}*/



	// Check if the king is in check
	int kingIdx = ( ( player == ME ) ? bitScanForward( newState->myKing ) : bitScanForward( newState->oppKing ) );
	int test = newState->isThreatened( kingIdx, to_idx, from_idx, player );
	if( test != NOT_THREATENED )
		{
		if( DEBUG_PRINT ) std::cout << "Puts King in check from idx: " << test << std::endl;
		piece->reset( to_idx );
		piece->set( from_idx );
		return;
		}
	
	// If we made it this far, the move is valid
	if( DEBUG_PRINT ) std::cout << "Is valid!" << std::endl;
	newState->calcScore();
	frontier.push_back( newState );

	return;
	}


/**************************************************************
* Calculate Score
* Hueristic evaluation function
**************************************************************/
void Chess::State::calcScore()
	{
	/*
	Bitboard* myBitboards[ 6 ]{ &myPawns, &myRooks, &myKnights, &myBishops, &myQueens, &myKing };
	score = 0;
	int i, idx;
	Bitboard pieces;
	for( i = 0; i < 6; i++ )
		{
		pieces = *myBitboards[ i ];
		while( ( idx = bitScanForward( pieces ) ) != -1 )
			{
			pieces.reset( idx );
			if( color == WHITE )
				idx = 63 - idx;
			score += ( squareVals[ i ] )[ idx ];
			}
		}
	return;
	*/
	
	Bitboard pawns = myPawns;
	int idx, i, pawnsInFile;

	Bitboard allMy = myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
	Bitboard allOpp = oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
	Bitboard all = allMy | allOpp;
	int dir = ( color == BLACK ? -1 : 1 );

	// Evaluate pwn structure
	int blockedPawns = 0;
	int doubledPawns = 0;
	int isolatedPawns = 0;
	while( ( idx = bitScanForward( pawns ) ) != -1 )
		{
		pawns.reset( idx );

		//Check is pawn is blocked
		if( testIdx( all, idx + ( dir * 8 ) ) )
			blockedPawns++;

		// Check if pawn is doubled
		pawnsInFile = 0;
		for( i = getFileNum( idx ); i <= 63; i += 8 )
			{
			if( myPawns.test( i ) )
				pawnsInFile++;
			}
		if( pawnsInFile > 1 )
			doubledPawns++;

		// Check if pawn is isolated
		pawnsInFile = 0;
		i = getFileNum( idx );
		if( i != 0 )
			{
			for( i - 1; i <= 63; i += 8 )
				{
				if( myPawns.test( i ) )
					pawnsInFile++;
				}
			}
		if( i != 7 && pawnsInFile == 0 )
			{
			for( i + 1; i <= 63; i += 8 )
				{
				if( myPawns.test( i ) )
					pawnsInFile++;
				}
			}
		if( pawnsInFile == 0 )
			isolatedPawns++;
	}

	// Calculate score
	score += KINGVAL * ( myKing.count() - oppKing.count() );
	score += QUEENVAL * ( myQueens.count() - oppQueens.count() );
	score += ROOKVAL * ( myRooks.count() - oppRooks.count() );
	score += KNIGHTVAL * ( myKnights.count() - oppKnights.count() );
	score += BISHOPVAL * ( myBishops.count() - oppBishops.count() );
	score += PAWNVAL * ( myPawns.count() - oppPawns.count() );
	score -= PAWNPENALTY * ( blockedPawns + doubledPawns + isolatedPawns );
	
	return;
	}


/**************************************************************
* Print Moves
* Outputs all moves in passed vector to the console in a
* user-readable format
**************************************************************/
void printMoves( std::vector<Chess::State*>* moves )
	{
	std::vector<Chess::State*>::iterator it = moves->begin();
	std::vector<Chess::State*>::iterator end = moves->end();
	int toIdx, fromIdx;
	std::cout << "Possible moves:" << std::endl;
	for( it; it != end; it++ )
		{
		toIdx = ( ( *it )->misc.to_ullong() & TOIDX_MASK ) >> TOIDX_BITSHIFT;
		fromIdx = ( ( *it )->misc.to_ullong() & FROMIDX_MASK ) >> FROMIDX_BITSHIFT;
		std::cout << "  " << ( char )( ( fromIdx % 8 ) + 'a' ) << 1 + fromIdx / 8
			<< " to " << ( char )( ( toIdx % 8 ) + 'a' ) << 1 + toIdx / 8
			<< " S:" << ( *it )->score << std::endl;
		}
	return;
	}