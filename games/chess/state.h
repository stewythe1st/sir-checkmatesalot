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
#define DEBUG_PRINT	false
enum { WHITE, BLACK	};
enum { ME, OPPONENT };
typedef enum { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING } PieceType;
// Misc indices
#define TOIDX_BITSHIFT		24
#define TOIDX_MASK			0x00000000FF000000
#define FROMIDX_BITSHIFT	32
#define FROMIDX_MASK		0x000000FF00000000


/******************************************************
* Types
******************************************************/
typedef std::bitset<64> Bitboard;
struct Chess::CondensedMove
	{
	Bitboard diff;
	Bitboard* parent;
	int score;
	CondensedMove( Bitboard* p, Bitboard d, int s ) { parent = p;	diff = d; score = s; };
	bool operator<( Chess::CondensedMove const& x ) const { return score > x.score; }
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

		Bitboard misc;

		int score;

		State* parent;

		bool inCheck;
		bool color;
		int invalid_from_idx;
		int invalid_to_idx;
		int en_passant;
		bool canCastleA;
		bool canCastleH;

		State( Chess::State * parent );
		State( Chess::AI* ai );
		State() {};
		~State() {};

		void Actions( std::vector<Chess::State*>& frontier, int player );
		int isThreatened( int idx, int to_idx, int from_idx, int player );
		void addMove( std::vector<Chess::State*>& frontier, int from_idx, int to_idx, Bitboard * piece, int player );
		void calcScore();
		Chess::State& operator= ( const Chess::State &rhs );

	};
	void printMoves( std::vector<Chess::State*>* moves );

#endif