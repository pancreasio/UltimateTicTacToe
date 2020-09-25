#include "UTTTGame.h"
#include "iostream"

using namespace std;

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

bool CheckInput(int input, Gamestate gamestate)
{
	if (input > 0 && input <= 9 && gamestate.cellArray[input - 1] == emptyCell)
		return true;
	else
		return false;
}

void UTTTGame::StartGame()
{
	char playerInputChar;
	int playerInputInt;
	gameEnded = false;
	currentPlayer = dot;

	for (int i = 0; i < 9; i++)
	{
		currentGamestate.cellArray[i] = emptyCell;
	}
}

bool UTTTGame::CheckLastMoveValid()
{
	return wasLastInputValid;
}

void UTTTGame::MakeInput(int input)
{
	if (!gameEnded)
	{
		bool validInput = false;
		validInput = CheckInput(input, currentGamestate);
		if (!validInput)
			wasLastInputValid = false;
		else
		{
			currentGamestate.cellArray[input - 1] = currentPlayer;
			wasLastInputValid = true;
			if (currentPlayer == dot)
				currentPlayer = cross;
			else currentPlayer = dot;

			if (CheckAll(currentGamestate, dot) || CheckAll(currentGamestate, cross) || RemainingMoves(currentGamestate) <= 0)
				gameEnded = true;
		}
	}
}

MatchState UTTTGame::GetCurrentResult()
{
	if (!gameEnded)
		return ongoing;
	if (CheckAll(currentGamestate, dot))
		return p1Won;
	else
		if (CheckAll(currentGamestate, cross))
			return p2Won;
		else
			return gameTied;
}

std::string UTTTGame::ReturnGamestate()
{
	string result;
	for (int i = 2; i >= 0; i--)
	{
		for (int i2 = 0; i2 < 3; i2++)
		{
			result += CellToChar(currentGamestate.cellArray[i * 3 + i2]) + " ";
		}
		result += "\n";
	}
	return result;
}
