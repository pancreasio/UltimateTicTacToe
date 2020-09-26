#include "MultiplayerClient.h"

#include <iostream>
using namespace std;

enum MessageType
{
	connectToServer = 0,
	setUsername = 1,
	connectedToRoom = 2,
	startGame = 3,
	sendMove = 4,
	recieveMove = 5,
	invalidMove = 6,
	endGame = 7,
	rematchInRoom = 8,
	quitRoom = 9
};

struct Message
{
	byte cmd;
	char data[256];
};

struct User
{
	char username[256];
	sockaddr_in adress;
};

enum MPGamestate
{
	waitingRoom, myTurn, enemyTurn, gameEnded, rematchScreen, exitGame
};

void MultiplayerClient::StartClient()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA data;

	Message msg;
	MPGamestate currentGameState = waitingRoom;
	if (WSAStartup(version, &data) == 0)
	{
		bool returnToMenu = false;
		string ipToUse("127.0.0.1");
		int portToUse = 8900;

		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(portToUse);
		inet_pton(AF_INET, ipToUse.c_str(), &server.sin_addr);

		SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

		string msgtest = "";

		char buf[1024];
		int serverSize = sizeof(server);

		msg.cmd = (byte)MessageType::connectToServer;
		memset(msg.data, 0, sizeof(msg.data));
		sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));
		cout << "attempting to connect to server..." << endl;
		memset(&msg, 0, sizeof(msg));

		int bytesIn = recvfrom(out, (char*)&msg, sizeof(msg), 0, (sockaddr*)&server, &serverSize);

		if (bytesIn == SOCKET_ERROR)
		{
			cerr << "Socket error, closing program..." << endl;
			return;
		}
		else
		{
			char serverInfo[256];
			strcpy_s(serverInfo, msg.data);
			if (!(MessageType)msg.cmd == connectToServer || serverInfo == "createUsername")
			{
				cout << "server error, closing program...";
				return;
			}
		}


		cout << "Connected to the Server!" << endl;
		cout << "Choose your username" << endl;

		cin >> msgtest;
		//getline(cin, msgtest);
		strcpy_s(msg.data, msgtest.c_str());

		msg.cmd = (byte)MessageType::setUsername;
		sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));

		while (!returnToMenu)
		{
			memset(buf, 0, sizeof(buf));
			memset(&msg, 0, sizeof(msg));
			int bytesIn = recvfrom(out, (char*)&msg, sizeof(msg), 0, (sockaddr*)&server, &serverSize);

			if (bytesIn == SOCKET_ERROR)
			{
				cerr << "Socket error, closing program..." << endl;
				return;
			}

			MessageType command = (MessageType)msg.cmd;

			switch (command)
			{
			case connectToServer:break;
			case setUsername:break;
			case connectedToRoom:
				cout << "connected to room!" << endl;
				currentGameState = waitingRoom;
				break;
			case startGame:
				cout << "game started!" << endl;
				if (msg.data[0] == '1')
				{
					currentGameState = myTurn;
					cout << "you are player 1" << endl;
				}
				if (msg.data[0] == '2')
				{
					currentGameState = enemyTurn;
					cout << "you are player 2" << endl;
				}
				break;
			case sendMove:
				system("cls");
				cout << msg.data << endl;
				break;
			case recieveMove:
				system("cls");
				cout << msg.data << endl;
				currentGameState = myTurn;
				break;
			case invalidMove:
				if (msg.data == "true")
					currentGameState = myTurn;
				else
					currentGameState = enemyTurn;
				break;
			case endGame:
				if (msg.data == "playerWon")
					cout << "you win!" << endl;
				if (msg.data == "playerLost")
					cout << "you suck!" << endl;
				if (msg.data == "matchTie")
					cout << "you both suck!" << endl;
				currentGameState = rematchScreen;
				break;
			case rematchInRoom:
				if (currentGameState == rematchInRoom)
					currentGameState = waitingRoom;
				break;
			case quitRoom: break;
			default:;
			}

			if(currentGameState == waitingRoom)
			{
				
			}
			if(currentGameState == myTurn)
			{
				char playerInputChar;
				int playerInputInt;
				bool validInput = false;
				while (!validInput)
				{
					cin >> playerInputChar;
					playerInputInt = (int)playerInputChar - 48;
					if (playerInputInt < 0 || playerInputInt>9)
					{
						cout << "invalid input" << endl;
					}
					else
						validInput = true;
				}
				msg.cmd = sendMove;
				msg.data[0] = playerInputChar;
				sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));
			}
			if(currentGameState == enemyTurn)
			{
				cout << "enemy turn" << endl;
			}
		}

	}

	WSACleanup();
	return;

}

void MultiplayerClient::RecieveString(SOCKET& socket, sockaddr_in& server)
{
	char buf[1024];
	int serverSize = sizeof(server);
	int bytesIn = recvfrom(socket, buf, sizeof(buf), 0, (sockaddr*)&server, &serverSize);

	if (bytesIn == SOCKET_ERROR)
	{
		cerr << "socket error, closing program..." << endl;
		return;
	}

	cout << buf << endl;
}