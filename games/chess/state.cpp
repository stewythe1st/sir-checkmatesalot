#include "ai.h"
#include "state.h"
#include "player.h"
#include "game.h"
#include "piece.h"
#include <map>

static bool isValidIdx(int idx);

Chess::State::State( Chess::Game* game, Chess::AI* ai)
{
	std::map<std::string, Bitboard*> pieceConvert;
	pieceConvert["myPawn"]		= &myPawns;
	pieceConvert["myRook"]		= &myRooks;
	pieceConvert["myKnight"]	= &myKnights;
	pieceConvert["myBishop"]	= &myBishops;
	pieceConvert["myQueen"]		= &myQueens;
	pieceConvert["myKing"]		= &myKing;
	pieceConvert["oppPawn"]		= &oppPawns;
	pieceConvert["oppRook"]		= &oppRooks;
	pieceConvert["oppKnight"]	= &oppKnights;
	pieceConvert["oppBishop"]	= &oppBishops;
	pieceConvert["oppQueen"]	= &oppQueens;
	pieceConvert["oppKing"]		= &oppKing;

	// Initialize all bitboards to zero
	myPawns = 0;
	myRooks = 0;
	myKnights = 0;
	myBishops = 0;
	myQueens = 0;
	myKing = 0;
	oppPawns = 0;
	oppRooks = 0;
	oppKnights = 0;
	oppBishops = 0;
	oppQueens = 0;
	oppKing = 0;

	std::vector<Chess::Piece*>::iterator piece;
	std::vector<Chess::Piece*>::iterator end;

	// Read in our color
	color = BLACK;
	if (ai->player->color == "White")
	{
		color = WHITE;
	}


	// Read in our pieces
	piece = ai->player->pieces.begin();
	end = ai->player->pieces.end();
	for (piece; piece != end; piece++)
	{
		(pieceConvert[("my" + (*piece)->type)])->set(getBitboardIdx((*piece)->rank, &(*piece)->file));
	}

	// Read in our opponent's pieces
	piece = ai->player->opponent->pieces.begin();
	end = ai->player->opponent->pieces.end();
	for (piece; piece != end; piece++)
	{
		(pieceConvert[ ( "opp" + (*piece)->type)])->set(getBitboardIdx((*piece)->rank, &(*piece)->file));
	}
	//print_bitboard(&myPawns);
}

int getBitboardIdx(int rank, std::string* file)
{
	//(*bitboard).set(((*file)[0] - 'a') + ((*rank - 1) * 8));
	return(((*file)[0] - 'a') + ((rank - 1) * 8));
}

void print_bitboard(Bitboard* bitboard)
{
	std::string str = "Val: " + bitboard->to_string() + "\n";
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
	str += "\n";
	std::cout << str;
}

void Chess::State::Actions(std::vector<Chess::CondensedMove>& moves)
{
	Bitboard allMy = myPawns | myKnights | myBishops | myRooks | myQueens | myKing;
	Bitboard allOpp = oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
	Bitboard all = allMy | allOpp;

	int dir = 1;
	if (color == BLACK)
		dir = -1;
		
	Bitboard temp = 0xFFFFFFFFFFFFFFFF;
	Bitboard temp2;
	int idx;
	while ((idx = bitScanForward((myPawns & temp).to_ullong())) != -1) {
		//std::cout << "Pawn at " << idx << std::endl;
		temp.reset(idx);

		// if pawn is in last terminal row (and can be promoted)
		if (!isValidIdx(idx + (2 * 8 * dir)))
		{
			continue; //add stuff here later
		}
		else
		{
			if (((idx + (7*dir)) / 8 == ((idx / 8) + dir)) && allOpp.test(idx + (7 * dir)) && !allMy.test(idx + (7 * dir)))
			{
				temp2 = 0;
				temp2.set(idx + 7 * dir);
				temp2.set(idx);
				moves.emplace_back(&myPawns, temp2);
				//std::cout << "Can move 7" << std::endl;
				//print_bitboard(&temp2);
			}
			if (!all.test(idx + (8 * dir)))
			{
				temp2 = 0;
				temp2.set(idx + 8 * dir);
				temp2.set(idx);
				moves.emplace_back(&myPawns, temp2);
				//std::cout << "Can move 8" << std::endl;
				//print_bitboard(&temp2);
				//print_bitboard(&all);
			}
			if (((color == WHITE && idx/8 == 1) || (color == BLACK && idx/8 == 6)) && (!all.test(idx + (16 * dir))))
			{
				temp2 = 0;
				temp2.set(idx + 16 * dir);
				temp2.set(idx);
				moves.emplace_back(&myPawns, temp2);
				//std::cout << "Can move 16" << std::endl;
				//print_bitboard(&temp2);
				//print_bitboard(&all);
			}
			if (((idx+(9*dir))/8 == ((idx/8)+dir)) && allOpp.test(idx + (9 * dir)) && !allMy.test(idx + (9 * dir)))
			{
				temp2 = 0;
				temp2.set(idx + 9 * dir);
				temp2.set(idx);
				moves.emplace_back(&myPawns, temp2);
				//std::cout << "Can move 9" << std::endl;
				//print_bitboard(&temp2);
			}
		}

	}
	/*std::vector<Chess::CondensedMove>::iterator move = moves.begin();
	std::vector<Chess::CondensedMove>::iterator end = moves.end();
	for (move; move != end; move++)
	{
		print_bitboard(&(*move).diff);
	}*/
	return;

}

const int index64[64] = {
	0,  1, 48,  2, 57, 49, 28,  3,
	61, 58, 50, 42, 38, 29, 17,  4,
	62, 55, 59, 36, 53, 51, 43, 22,
	45, 39, 33, 30, 24, 18, 12,  5,
	63, 47, 56, 27, 60, 41, 37, 16,
	54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10,
	25, 14, 19,  9, 13,  8,  7,  6
};
/**
* bitScanForward
* @author Martin Läuter (1997)
*         Charles E. Leiserson
*         Harald Prokop
*         Keith H. Randall
* "Using de Bruijn Sequences to Index a 1 in a Computer Word"
* @param bb bitboard to scan
* @precondition bb != 0
* @return index (0..63) of least significant one bit
* @source https://chessprogramming.wikispaces.com/Bitscan
*/
int bitScanForward(unsigned long long bb) {
	const unsigned long long debruijn64 = 0x03f79d71b4cb0a89;
	if(bb == 0)
		return -1;
	return index64[((bb & -bb) * debruijn64) >> 58];
}

static bool isValidIdx(int idx)
{
	return( (idx <= 64) && (idx >= 0));
}