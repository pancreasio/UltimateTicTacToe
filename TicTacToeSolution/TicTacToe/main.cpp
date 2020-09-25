#include <iostream>
#include  "Singleplayer.h"

using namespace std;

enum Gamestate {menu, singleplayer, multiplayer, exitgame};

void ShowOptions()
{
	cout << "Welcome to Ultimate Tic Tac Toe!" << endl;
	cout << "Select a gamemode:" << endl;
	cout << "1: Singleplayer" << endl;
	cout << "2: Multiplayer" << endl;
	cout << "3: Exit game" << endl;
}

Gamestate CheckOptions(int input)
{
	Gamestate result;
	if (!input < 1 || !input>3)
	{
		switch (input)
		{
		case 1:
			result = singleplayer;
			break;
		case 2:
			result = multiplayer;
			break;
		case 3:
			result = exitgame;
			break;
		}
	}
	else
	{
		cout << "invalid option, please enter a valid gamemode" << endl;
		result = menu;
	}
	return result;
}
void main()
{
	bool gameended = false;
	Gamestate currentGameState = menu;
	char playerInputChar;
	int playerInput;
	Singleplayer game;
	ShowOptions();

	while (!gameended)
	{
		cin >> playerInputChar;

		playerInput = (int)playerInputChar - 48;
		if (playerInput < 0 || playerInput>9)
		{
			cout << "invalid option, please enter a valid gamemode" << endl;
		}
		else
		{
			currentGameState = CheckOptions(playerInput);
		}
		
		switch (currentGameState)
		{
		case menu:
				break;
		case singleplayer:
			system("cls");
			game.PlayGame();
			system("cls");
			ShowOptions();
			break;
		case multiplayer:
			cout << "multiplayer currently unavailable" << endl;
			break;
		case exitgame:
			gameended = true;
		}
		if (currentGameState == exitgame)
			gameended = true;
	}
}