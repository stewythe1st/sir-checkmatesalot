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
#include <boost/functional/hash.hpp>


/******************************************************
* Compiler Constants
******************************************************/
#define DEBUG_PRINT			false
#define TOIDX_BITSHIFT		24
#define TOIDX_MASK			0x00000000FF000000
#define FROMIDX_BITSHIFT	32
#define FROMIDX_MASK		0x000000FF00000000
#define EN_PASSANT_MASK		0x0000FF0000FF0000
#define CASTLE_MASK			0x8100000000000081

enum { WHITE, BLACK };
enum { ME, OPPONENT };


/******************************************************
* Types
******************************************************/
typedef std::bitset<64> Bitboard;
typedef enum { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING } PieceType;


/******************************************************
* Public Utility Functions
******************************************************/
int bitScanForward( Bitboard bb );
int getBitboardIdx( int rank, std::string file );
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
		int historyVal;

		State* parent;

		bool color;

		State( Chess::State * parent );
		State( Chess::AI* ai );
		State() {};
		~State() {};

		void Actions( std::vector<Chess::State*>& frontier, int player );
		int isThreatened( int idx, int to_idx, int from_idx, int player );
		void addMove( std::vector<Chess::State*>& frontier, int from_idx, int to_idx, Bitboard * piece, int player );
		void calcScore();
		Chess::State& operator= ( Chess::State &rhs );
		bool operator == ( const Chess::State & other ) const;

		// Mutators
		void setHistoryVal( int x ) { historyVal = x; };
		int getHistoryVal() { return historyVal; };

	};
	void printMoves( std::vector<Chess::State*>* moves );


	/**************************************************************
	* State Hash Functor
	* Overloads std::hash to handle State types. Relies on Boost's
	* hash_combine() function. Only hashs board layout directly;
	* other members of state are irrelevant for hashing.
	**************************************************************/
	struct StateHash
		{
		std::size_t operator()( const Chess::State& s ) const
			{
			std::size_t seed = 0;
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.myPawns ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.myRooks ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.myKnights ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.myBishops ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.myQueens ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.myKing ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.oppPawns ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.oppRooks ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.oppKnights ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.oppBishops ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.oppQueens ) );
			boost::hash_combine( seed, std::hash<Bitboard>{}( s.oppKing ) );
			return seed;
			}
		};


	/**************************************************************
	* State Sort Functor
	* Allows std::sort to handle State types. Bases the comparison
	* on each state's history table value, returning the higher
	* value to result in a decending sort
	**************************************************************/
	struct StateSort
		{
		inline bool operator() ( const Chess::State* x, const Chess::State* y )
			{
			return( x->historyVal > y->historyVal );
			}
		};

#endif