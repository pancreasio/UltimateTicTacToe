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

	UTTTGame() {};
	~UTTTGame();
	void StartGame();
	bool CheckLastMoveValid();
	void MakeInput(int input);
	MatchState GetCurrentResult();
	std::string ReturnGamestate();

};

#endif