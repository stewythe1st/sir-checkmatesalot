#ifndef JOUEUR_CHESS_STATE_H
#define JOUEUR_CHESS_STATE_H

#include "chess.h"
#include "gameObject.h"
#include <bitset>

typedef std::bitset<64> Bitboard;

class Chess::State : public Chess::GameObject 
{
	friend Chess::GameManager;

	protected:
		
	public:
		Bitboard whitePawns;
		Bitboard whiteKnights;
		Bitboard whiteBishops;
		Bitboard whiteRooks;
		Bitboard whiteQueens;
		Bitboard whiteKing;

		Bitboard blackPawns;
		Bitboard blackKnights;
		Bitboard blackBishops;
		Bitboard blackRooks;
		Bitboard blackQueens;
		Bitboard blackKing;

		State(Chess::Game* game);
		State() {};
		~State() {};

};

#endif