#ifndef UTTTGAME_H
#define UTTTGAME_H
#include <string>

enum Cell
{
	emptyCell, dot, cross
};

struct Gamestate
{
	Cell cellArray[9];
};

enum MatchState
{
	ongoing, p1Won, p2Won, gameTied
};

enum RematchState{
	tryingRematch, rematchRejected, waiting, playing
};

class UTTTGame
{
protected:
	Gamestate currentGamestate;
	bool wasLastInputValid;
	bool gameEnded;
	Cell currentPlayer;
	char CellToChar(Cell input)
	{
		char output;
		switch (input)
		{
		case emptyCell:
			output = '.';
			break;
		case dot:
			output = 'O';
			break;
		case cross:
			output = 'X';
			break;
		}
		return output;
	}

public:
	bool player1connected;
	RematchState rematchState;
	std::string player1Name;
	bool player2connected;
	std::string player2Name;
	UTTTGame() { player1connected = false; player2connected = false; };
	~UTTTGame();
	void StartGame();
	bool CheckLastMoveValid();
	void MakeInput(int input);
	MatchState GetCurrentResult();
	std::string ReturnGamestate();

};

#endif