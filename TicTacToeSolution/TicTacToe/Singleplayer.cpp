#include "Singleplayer.h"

#include "iostream"

using namespace std;

enum Cell
{
	emptyCell, dot, cross
};

struct Gamestate
{
	Cell cellArray[9];
};

int Min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

int Max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

bool CheckVertical(Gamestate gamestate, Cell checkingPlayer)
{
	if (gamestate.cellArray[6] == checkingPlayer && gamestate.cellArray[6] == gamestate.cellArray[3] && gamestate.cellArray[3] == gamestate.cellArray[0])
		return true;
	else
		if (gamestate.cellArray[7] == checkingPlayer && gamestate.cellArray[7] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[1])
			return true;
		else
			if (gamestate.cellArray[8] == checkingPlayer && gamestate.cellArray[8] == gamestate.cellArray[5] && gamestate.cellArray[5] == gamestate.cellArray[2])
				return true;
			else
				return false;
}


bool CheckHorizontal(Gamestate gamestate, Cell checkingPlayer)
{
	if (gamestate.cellArray[6] == checkingPlayer && gamestate.cellArray[6] == gamestate.cellArray[7] && gamestate.cellArray[7] == gamestate.cellArray[8])
		return true;
	else
		if (gamestate.cellArray[3] == checkingPlayer && gamestate.cellArray[3] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[5])
			return true;
		else
			if (gamestate.cellArray[0] == checkingPlayer && gamestate.cellArray[0] == gamestate.cellArray[1] && gamestate.cellArray[1] == gamestate.cellArray[2])
				return true;
			else
				return false;
}

bool CheckCrosses(Gamestate gamestate, Cell checkingPlayer)
{
	if (gamestate.cellArray[6] == checkingPlayer && gamestate.cellArray[6] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[2])
		return true;
	else
		if (gamestate.cellArray[8] == checkingPlayer && gamestate.cellArray[8] == gamestate.cellArray[4] && gamestate.cellArray[4] == gamestate.cellArray[0])
			return true;
		else
			return false;
}

bool CheckAll(Gamestate gamestate, Cell checkingPlayer)
{
	return CheckHorizontal(gamestate, checkingPlayer) || CheckVertical(gamestate, checkingPlayer) || CheckCrosses(gamestate, checkingPlayer);
}

int RemainingMoves(Gamestate gamestate)
{
	int result = 0;
	for (int i = 0; i < 9; i++)
	{
		if (gamestate.cellArray[i] == emptyCell)
			result++;
	}
	return  result;
}

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

void RenderGamestate(Gamestate gamestate)
{
	for (int i = 2; i >= 0; i--)
	{
		for (int i2 = 0; i2 < 3; i2++)
		{
			cout << CellToChar(gamestate.cellArray[i * 3 + i2]) << "  ";
		}
		cout << endl;
	}
}

Gamestate MakeMove(Gamestate previousGamestate, Cell player, int cycleNumber)
{
	int emptyCellsFound = 0;
	for (int i = 0; i < 9; i++)
	{
		if (previousGamestate.cellArray[i] == emptyCell)
		{
			emptyCellsFound++;
		}
		if (emptyCellsFound - 1 >= cycleNumber)
		{
			previousGamestate.cellArray[i] = player;
			i = 9;
		}
	}
	return previousGamestate;
}


int MiniMaxNodes(Gamestate gamestate, Cell playerToMove)
{

	if (CheckAll(gamestate, Cell::dot))
		return -1;

	if (CheckAll(gamestate, Cell::cross))
		return 1;

	if (RemainingMoves(gamestate) <= 0)
		return 0;


	if (playerToMove == cross)
	{
		int bestValue = -100;
		for (int i = 0; i < RemainingMoves(gamestate); i++)
		{
			bestValue = Max(bestValue, MiniMaxNodes(MakeMove(gamestate, cross, i), dot));
		}
		return bestValue;
	}
	if (playerToMove == dot)
	{
		int bestValue = 100;
		for (int i = 0; i < RemainingMoves(gamestate); i++)
		{
			bestValue = Min(bestValue, MiniMaxNodes(MakeMove(gamestate, dot, i), cross));
		}
		return bestValue;
	}
}

Gamestate BestNextMove(Gamestate currentGamestate)
{
	int bestValue = -100;
	Gamestate nextMove = currentGamestate;


	int remainingCells = RemainingMoves(currentGamestate);
	for (int i = 0; i < remainingCells; i++)
	{
		Gamestate hypoteticalMove = MakeMove(currentGamestate, cross, i);
		//RenderGamestate(hypoteticalMove);
		int resultantValue = MiniMaxNodes(hypoteticalMove, dot);
		if (bestValue < resultantValue)
		{
			nextMove = hypoteticalMove;
			bestValue = resultantValue;
		}
	}
	return nextMove;
}

bool CheckInput(int input, Gamestate gamestate)
{
	if (input > 0 && input <= 9 && gamestate.cellArray[input - 1] == emptyCell)
		return true;
	else
		return false;
}

void Singleplayer::PlayGame()
{
	char playerInputChar;
	int playerInputInt;
	bool gameEnded = false;
	Gamestate currentGamestate;

	for (int i = 0; i < 9; i++)
	{
		currentGamestate.cellArray[i] = emptyCell;
	}

	while (!gameEnded)
	{
		cout << "input move" << endl;
		bool validInput = false;
		while (!validInput)
		{
			cin >> playerInputChar;
			playerInputInt = (int)playerInputChar - 48;
			if(playerInputInt<0||playerInputInt>9)
			{
				cout << "invalid input" << endl;
			}
			else
			{
				validInput = CheckInput(playerInputInt, currentGamestate);
				if(!validInput)
					cout << "invalid input" << endl;
			}

		}
		currentGamestate.cellArray[playerInputInt - 1] = dot;

		if (!CheckAll(currentGamestate, dot) || !CheckAll(currentGamestate, cross) || RemainingMoves(currentGamestate) > 0)
			currentGamestate = BestNextMove(currentGamestate);

		RenderGamestate(currentGamestate);

		if (CheckAll(currentGamestate, dot) || CheckAll(currentGamestate, cross) || RemainingMoves(currentGamestate) <= 0)
			gameEnded = true;
	}

	if (CheckAll(currentGamestate, dot))
		cout << endl << "humankind survived, for now" << endl;
	else
		if (CheckAll(currentGamestate, cross))
			cout << endl << "THE IA MASTERMIND OBLITERATED THE HUMAN RACE" << endl;
		else
			cout << endl << "the war for humanity continues..." << endl;
}