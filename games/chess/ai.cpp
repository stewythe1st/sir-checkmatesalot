// This is where you build your AI for the Chess game.

#include "ai.h"


/// <summary>
/// This returns your AI's name to the game server. Just replace the string.
/// </summary>
/// <returns>string of you AI's name.</returns>
std::string Chess::AI::getName()
{
    return "Chess C++ Player"; // REPLACE THIS WITH YOUR TEAM NAME!
}

/// <summary>
/// This is automatically called when the game first starts, once the Game object and all GameObjects have been initialized, but before any players do anything.
/// </summary>
void Chess::AI::start()
{
    // This is a good place to initialize any variables you add to your AI, or start tracking game objects.
}

/// <summary>
/// This is automatically called every time the game (or anything in it) updates.
/// </summary>
void Chess::AI::gameUpdated()
{
    // If a function you call triggers an update this will be called before that function returns.
}

/// <summary>
/// This is automatically called when the game ends.
/// </summary>
/// <param name="won">true if your player won, false otherwise</param>
/// <param name="reason">a string explaining why you won or lost</param>
void Chess::AI::ended(bool won, std::string reason)
{
    // You can do any cleanup of you AI here, or do custom logging. After this function returns the application will close.
}


/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool Chess::AI::runTurn()
{
    // Here is where you'll want to code your AI.

    // We've provided sample code that:
    //    1) prints the board to the console
    //    2) prints the opponent's last move to the console
    //    3) prints how much time remaining this AI has to calculate moves
    //    4) makes a random (and probably invalid) move.

    // 1) print the board to the console
    for (int rank = 9; rank >= -1; rank--)
    {
        std::string str = "";
        if (rank == 9 || rank == 0) // then the top or bottom of the board
        {
            str = "   +------------------------+";
        }
        else if (rank == -1) // then show the ranks
        {
            str = "     a  b  c  d  e  f  g  h";
        }
        else // board
        {
            str += " ";
            str += std::to_string(rank);
            str += " |";
            // fill in all the files with pieces at the current rank
            for (int fileOffset = 0; fileOffset < 8; fileOffset++)
            {
                std::string file(1, (char)(((int)"a"[0]) + fileOffset)); // start at a, with with file offset increasing the char;
                Chess::Piece* currentPiece = nullptr;
                for (auto piece : this->game->pieces)
                {
                    if (piece->file == file && piece->rank == rank) // then we found the piece at (file, rank)
                    {
                        currentPiece = piece;
                        break;
                    }
                }

                char code = '.'; // default "no piece";
                if (currentPiece != nullptr)
                {
                    code = currentPiece->type[0];

                    if (currentPiece->type == "Knight") // 'K' is for "King", we use 'N' for "Knights"
                    {
                        code = 'N';
                    }

                    if (currentPiece->owner->id == "1") // the second player (black) is lower case. Otherwise it's upppercase already
                    {
                        code = tolower(code);
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

    // 2) print the opponent's last move to the console
    if (this->game->moves.size() > 0) {
        std::cout << "Opponent's Last Move: '" << this->game->moves[this->game->moves.size() - 1]->san << "'" << std::endl;
    }

    // 3) print how much time remaining this AI has to calculate moves
    std::cout << "Time Remaining: " << this->player->timeRemaining << " ns" << std::endl;

    // 4) make a random (and probably invalid) move.
    srand(time(NULL));
    auto randomPiece = this->player->pieces[rand() % this->player->pieces.size()];
    std::string randomFile(1, (char)(((int)"a"[0]) + (rand() % 8)));
    int randomRank = (rand() % 8) + 1;
    

	// Build initial state for this move
	Chess::State initial( this->game, this );
	std::vector<Chess::CondensedMove> moves;

	// Determine possible actions from initial
	initial.Actions( moves );

	// Pick a random action and execute it
	executeMove(&moves[rand() % moves.size()] );

	//randomPiece->move(randomFile, randomRank);

    return true; // to signify we are done with our turn.
}

bool Chess::AI::executeMove(Chess::CondensedMove* move) 
{
	std::vector<Chess::Piece*>::iterator piece = this->player->pieces.begin();
	std::vector<Chess::Piece*>::iterator end = this->player->pieces.end();
	//std::cout << "Chosen move: " << std::endl;
	//print_bitboard(&move->diff);
	for (piece; piece != end; piece++)
	{
		int idx = getBitboardIdx((*piece)->rank, &(*piece)->file);
		if (move->diff.test(idx))
		{
			//std::cout << "Found at idx: " << idx << std::endl;
			int to_idx = bitScanForward(move->diff.reset(idx).to_ullong());
			//std::cout << "Moving to idx: " << to_idx << std::endl;
			std::string to_file(1,(char)(to_idx%8)+'a');
			int to_rank = 1+to_idx/8;
			return (*piece)->move(to_file, to_rank, "Queen");
		}
	}
	return false;
}
