#ifndef MULTIPLAYERSERVER_H
#define MULTIPLAYERSERVER_H

#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

class MultiplayerServer
{
public:
	void StartSertver();
	void RecieveString(SOCKET &socket, sockaddr_in &server);
};

#endif