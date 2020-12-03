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

string getEnemy(User* player)
{
	if (player->currentGame->player1Name == player->username)
	{
		return player->currentGame->player2Name;
	}
	else
		return player->currentGame->player1Name;
}

void MultiplayerServer::StartSertver()
{
	srand(time(0));

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
	while (!shouldClose)
	{
		memset(buf, 0, sizeof(buf));
		memset(&msg, 0, sizeof(msg));

		int bytesIn = recvfrom(listening, (char*)&msg, sizeof(msg), 0, (sockaddr*)&client, &clientSize);

		if (bytesIn == SOCKET_ERROR)
		{
			cerr << "data error, closing program..." << endl;
			return;
		}

		if ((MessageType)msg.cmd == connectToServer)
		{
			cout << "new user found" << endl;
			User* user = new User;
			user->adress = client;
			users.push_back(user);

			msg.cmd = (byte)MessageType::connectToServer;
			strcpy_s(msg.data, "createUsername");
			memset(msg.data, 0, sizeof(msg.data));
			sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&client, sizeof(client));
		}

		if ((MessageType)msg.cmd == setUsername)
		{
			bool userExists = false;
			for (int i = 0; i < users.size(); i++)
			{
				sockaddr* clientAddress = (sockaddr*)&client;
				sockaddr* userAddress = (sockaddr*)&users[i]->adress;
				if (memcmp(clientAddress, userAddress, sizeof(clientAddress)) == 0)
				{
					strcpy_s(users[i]->username, msg.data);
					cout << "user " << i << " chose the username: " + (string)msg.data << endl;
					bool foundOpenRoom = false;
					for (int i2 = 0; i2 < rooms.size(); i2++)
					{
						if (!rooms[i2]->player2connected)
						{
							users[i]->currentGame = rooms[i2];
							rooms[i2]->player2connected = true;
							rooms[i2]->player2Name = users[i]->username;
							rooms[i2]->StartGame();
							foundOpenRoom = true;
							cout << "allocated both players in room" << endl;
							msg.cmd = (byte)MessageType::connectedToRoom;
							strcpy_s(msg.data, "");
							sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));
							for (int i3 = 0; i3 < users.size(); i3++)
							{
								if (users[i3]->username == rooms[i2]->player1Name)
								{
									int coin = rand() % 2;
									msg.cmd = (byte)MessageType::startGame;
									rooms[i2]->StartGame();
									if (coin == 0)
										strcpy_s(msg.data, "1");
									else
										strcpy_s(msg.data, "2");

									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));
									msg.cmd = (byte)MessageType::startGame;
									if (coin == 0)
										strcpy_s(msg.data, "2");
									else
										strcpy_s(msg.data, "1");

									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i3]->adress, sizeof(users[i3]->adress));
									cout << "game between " << users[i]->username << " and " << users[i3]->username << " started" << endl;
									i3 = users.size();
								}
							}
							i2 = rooms.size();
						}
					}
					if (!foundOpenRoom)
					{
						msg.cmd = (byte)MessageType::connectedToRoom;
						strcpy_s(msg.data, "");
						sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&client, sizeof(client));
						UTTTGame* newRoom = new UTTTGame();
						users[i]->currentGame = newRoom;
						newRoom->player1connected = true;
						newRoom->player1Name = users[i]->username;
						rooms.push_back(newRoom);
					}
					userExists = true;
					i = users.size();
				}
			}
		}
		if ((MessageType)msg.cmd == sendMove)
		{
			for (int i = 0; i < users.size(); i++)
			{
				sockaddr* clientAddress = (sockaddr*)&client;
				sockaddr* userAddress = (sockaddr*)&users[i]->adress;
				if (memcmp(clientAddress, userAddress, sizeof(clientAddress)) == 0)
				{
					users[i]->currentGame->MakeInput((int)msg.data[0] - '0');
					if (!users[i]->currentGame->CheckLastMoveValid())
					{
						msg.cmd = (byte)MessageType::invalidMove;
						strcpy_s(msg.data, "1");
						sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));
					}
					else
					{
						UTTTGame* currentGame = users[i]->currentGame;
						string enemyName;
						
						MatchState matchState = currentGame->GetCurrentResult();
						if(matchState == ongoing)
						{
							msg.cmd = (byte)MessageType::sendMove;
							strcpy_s(msg.data, users[i]->currentGame->ReturnGamestate().c_str());
							sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));

							msg.cmd = (byte)MessageType::recieveMove;
							strcpy_s(msg.data, users[i]->currentGame->ReturnGamestate().c_str());

							currentGame->player1Name == users[i]->username ? enemyName = currentGame->player2Name :
								enemyName = currentGame->player1Name;

							for (int i2 = 0; i2 < users.size(); i2++)
							{
								if (users[i2]->username == enemyName)
								{
									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i2]->adress, sizeof(users[i2]->adress));
									i2 = users.size();
								}
							}
						}
						else
						{
							currentGame->player1Name == users[i]->username ? enemyName = currentGame->player2Name :
								enemyName = currentGame->player1Name;
							
							bool isUser1Player1 = currentGame->player1Name == users[i]->username;
							msg.cmd = (byte)MessageType::endGame;
							
							switch (currentGame->GetCurrentResult())
							{
							case ongoing:
								cout << "xddd" << endl;
								break;
							case p1Won:	
								if(isUser1Player1)
								{
									strcpy_s(msg.data, "1");
									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));

									strcpy_s(msg.data, "2");
									for (int i2 = 0; i2 < users.size(); i2++)
									{
										if (users[i2]->username == enemyName)
										{
											sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i2]->adress, sizeof(users[i2]->adress));
											i2 = users.size();
										}
									}
								}
								else
								{
									strcpy_s(msg.data, "2");
									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));

									strcpy_s(msg.data, "1");
									for (int i2 = 0; i2 < users.size(); i2++)
									{
										if (users[i2]->username == enemyName)
										{
											sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i2]->adress, sizeof(users[i2]->adress));
											i2 = users.size();
										}
									}
								}
								break;
							case p2Won:
								if (!isUser1Player1)
								{
									strcpy_s(msg.data, "1");
									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));

									strcpy_s(msg.data, "2");
									for (int i2 = 0; i2 < users.size(); i2++)
									{
										if (users[i2]->username == enemyName)
										{
											sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i2]->adress, sizeof(users[i2]->adress));
											i2 = users.size();
										}
									}
								}
								else
								{
									strcpy_s(msg.data, "2");
									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));

									strcpy_s(msg.data, "1");
									for (int i2 = 0; i2 < users.size(); i2++)
									{
										if (users[i2]->username == enemyName)
										{
											sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i2]->adress, sizeof(users[i2]->adress));
											i2 = users.size();
										}
									}
								}
								break;
							case gameTied:
								strcpy_s(msg.data, "3");
								sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));
								for (int i2 = 0; i2 < users.size(); i2++)
								{
									if (users[i2]->username == enemyName)
									{
										sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i2]->adress, sizeof(users[i2]->adress));
										i2 = users.size();
									}
								}
								break;
							}
						}							
					}

					i = users.size();
				}
			}
		}
		if((MessageType)msg.cmd == rematchInRoom)
		{
			for (int i = 0; i < users.size(); i++)
			{
				sockaddr* clientAddress = (sockaddr*)&client;
				sockaddr* userAddress = (sockaddr*)&users[i]->adress;
				if (memcmp(clientAddress, userAddress, sizeof(clientAddress)) == 0)
				{
					if(users[i]->currentGame->rematchState == waiting)
					{
						users[i]->currentGame->rematchState = tryingRematch;
					}
					else
					{
						if(users[i]->currentGame->rematchState == tryingRematch)
						{
							UTTTGame* currentGame = users[i]->currentGame;
							currentGame->StartGame();
							msg.cmd = (byte)MessageType::rematchInRoom;
							strcpy_s(msg.data, "");
							sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));

							string enemyName = "";							
							currentGame->player1Name == users[i]->username ? enemyName = currentGame->player2Name :
								enemyName = currentGame->player1Name;
							
							for (int i3 = 0; i3 < users.size(); i3++)
							{
								if (users[i3]->username == enemyName)
								{
									int coin = rand() % 2;
									msg.cmd = (byte)MessageType::startGame;
									currentGame->StartGame();
									if (coin == 0)
										strcpy_s(msg.data, "1");
									else
										strcpy_s(msg.data, "2");

									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i]->adress, sizeof(users[i]->adress));
									msg.cmd = (byte)MessageType::startGame;
									if (coin == 0)
										strcpy_s(msg.data, "2");
									else
										strcpy_s(msg.data, "1");

									sendto(listening, (char*)&msg, sizeof(Message), 0, (sockaddr*)&users[i3]->adress, sizeof(users[i3]->adress));
									cout << "game between " << users[i]->username << " and " << users[i3]->username << " started" << endl;
									i3 = users.size();
								}
							}
						}
					}
					i = users.size();
				}
			}
		}
	}
}

void MultiplayerServer::RecieveString(SOCKET& socket, sockaddr_in& server)
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