#include "MultiplayerServer.h"

#include <ctime>
#include <iostream>
#include <vector>

#include "UTTTGame.h"

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
	UTTTGame* currentGame;
	bool wantsToRematch = false;
};

void MultiplayerServer::StartSertver()
{
	srand(time(NULL));

	WSADATA data;
	WORD ver = MAKEWORD(2, 2);

	Message msg;
	vector<User*> users;
	vector<UTTTGame*> rooms;

	int wsOk = WSAStartup(ver, &data);
	if (wsOk != 0)
	{
		cerr << "Winsock error, closing program..." << endl;
		return;
	}

	SOCKET listening = socket(AF_INET, SOCK_DGRAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Invalid socket, closing program..." << endl;
		return;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8900);
	inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr); // ip de loopback

	int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (bindResult == SOCKET_ERROR)
	{
		cerr << "Bind error, closing program..." << endl;
		return;
	}

	char buf[1024];
	sockaddr_in client;
	int clientSize = sizeof(client);

	bool shouldClose = false;
	while(!shouldClose)
	{
		memset(buf, 0, sizeof(buf));
		memset(&msg, 0, sizeof(msg));

		int bytesIn = recvfrom(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, &clientSize);

		if (bytesIn == SOCKET_ERROR)
		{
			cerr << "data error, closing program..." << endl;
			return;
		}

		if((MessageType)msg.cmd == connectToServer)
		{
			cout << "funca" << endl;
		}
	}
}

void MultiplayerServer::RecieveString(SOCKET& socket, sockaddr_in& server)
{
}
