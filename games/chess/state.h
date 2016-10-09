/**************************************************************
* state.h
* Declarations for the chess AI state class
* CS5400, FS 2016
* Stuart Miller
**************************************************************/
#ifndef JOUEUR_CHESS_STATE_H
#define JOUEUR_CHESS_STATE_H

/******************************************************
* Includes
******************************************************/
#include "chess.h"
#include "gameObject.h"
#include <bitset>


/******************************************************
* Compiler Constants
******************************************************/
#define WHITE 1
#define BLACK 0


/******************************************************
* Types
******************************************************/
typedef std::bitset<64> Bitboard;
struct Chess::CondensedMove
	{
	Bitboard diff;
	Bitboard* parent;
	CondensedMove( Bitboard* p, Bitboard d ) { parent = p;	diff = d; };
	};


/******************************************************
* Public Utility Functions
******************************************************/
int bitScanForward( Bitboard bb );
int getBitboardIdx( int rank, std::string* file );
void print_bitboard( Bitboard* bitboard );


/******************************************************
* State Class
******************************************************/
class Chess::State: public Chess::GameObject
	{
	friend Chess::GameManager;

	protected:

	public:
		Bitboard myPawns;
		Bitboard myKnights;
		Bitboard myBishops;
		Bitboard myRooks;
		Bitboard myQueens;
		Bitboard myKing;

		Bitboard oppPawns;
		Bitboard oppKnights;
		Bitboard oppBishops;
		Bitboard oppRooks;
		Bitboard oppQueens;
		Bitboard oppKing;

		bool inCheck;
		bool color;
		int invalid_from_idx;
		int invalid_to_idx;

		State( Chess::AI* ai );
		State() {};
		~State() {};

		void Actions( std::vector<Chess::CondensedMove>& moves );
		bool testCheck( int from_idx, int to_idx );
		void addMove( std::vector<Chess::CondensedMove>& moves, int from_idx, int to_idx );

	};

#endif