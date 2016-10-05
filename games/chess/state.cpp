#include "state.h"
#include "player.h"
#include "game.h"
#include "piece.h"
#include <map>

static void update_bitboard(Bitboard* bitboard, int* rank, std::string* file);

static void print_bitboard(Bitboard* bitboard);

Chess::State::State( Chess::Game* game)
{
	std::map<std::string, Bitboard*> pieceConvert;
	pieceConvert["WhitePawn"]	= &whitePawns;
	pieceConvert["WhiteRook"]	= &whiteRooks;
	pieceConvert["WhiteKnight"] = &whiteKnights;
	pieceConvert["WhiteBishop"] = &whiteBishops;
	pieceConvert["WhiteQueen"]	= &whiteQueens;
	pieceConvert["WhiteKing"]	= &whiteKing;
	pieceConvert["BlackPawn"]	= &blackPawns;
	pieceConvert["BlackRook"]	= &blackRooks;
	pieceConvert["BlackKnight"] = &blackKnights;
	pieceConvert["BlackBishop"] = &blackBishops;
	pieceConvert["BlackQueen"]	= &blackQueens;
	pieceConvert["BlackKing"]	= &blackKing;

	whitePawns = 0;
	whiteRooks = 0;
	whiteKnights = 0;
	whiteBishops = 0;
	whiteQueens = 0;
	whiteKing = 0;
	blackPawns = 0;
	blackRooks = 0;
	blackKnights = 0;
	blackBishops = 0;
	blackQueens = 0;
	blackKing = 0;

	std::vector<Chess::Player*>::iterator player = game->players.begin();
	for (player; player != game->players.end(); player++)
	{
		std::vector<Chess::Piece*>::iterator piece = (*player)->pieces.begin();
		for (piece; piece != (*player)->pieces.end(); piece++)
		{
			update_bitboard( pieceConvert[ ( (*player)->color + (*piece)->type ) ], &(*piece)->rank, &(*piece)->file );
		}
	}
}

static void update_bitboard(Bitboard* bitboard, int* rank, std::string* file)
{
	(*bitboard).set(((*file)[0] - 'a') + ((*rank - 1) * 8));
}

static void print_bitboard(Bitboard* bitboard)
{
	std::string str = "";
	for (int i = 7; i >= 0; i--)
	{
		for (int j = 0; j <= 7; j++)
		{
			if ((*bitboard).test(j + (i * 8)))
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
	std::cout << str;
}