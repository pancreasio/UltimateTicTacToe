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
			if(!(MessageType)msg.cmd==connectToServer)
			{
				return;
			}
		}
		
		
		cout << "Connected to the Server!"<<endl;
		cout << "Choose your username" << endl;

		getline(cin, msgtest);
		strcpy_s(msg.data, msgtest.c_str());

		msg.cmd = (byte)MessageType::setUsername;
		sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));

		while(!returnToMenu)
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
				if (currentGameState == waitingRoom)
				{
					if (msg.data == "firstMove")
						currentGameState = myTurn;
					else
						currentGameState = enemyTurn;
				}
					break;
			case sendMove: break;
			case recieveMove:
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
			default: ;
			}

			switch(currentGameState)
			{
			case waitingRoom:
				break;
			case myTurn:
				char playerInputChar;
				int playerInputInt;
				getline(cin, msgtest);
				playerInputChar = msgtest[0];
				strcpy_s(msg.data, msgtest.c_str());
				cin >> playerInputChar;
				playerInputInt = (int)playerInputChar - 48;
				if (playerInputInt < 0 || playerInputInt>9)
				{
					cout << "invalid input" << endl;
				}
				else
				{
					msg.cmd = sendMove;
					strcpy_s(msg.data, msgtest.c_str());
					sendto(out, (char*)&msg, sizeof(Message), 0, (sockaddr*)&server, sizeof(server));
				}

				break;
			case enemyTurn:
				cout << "enemy turn" << endl;
				break;
			case gameEnded:
				return;
				break;
			case rematchScreen: break;
			case exitGame: break;
				return;
			default: ;
			}
			
			if (msg.data != " ")
				cout << msg.data << endl;
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