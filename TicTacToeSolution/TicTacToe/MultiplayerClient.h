#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

class MultiplayerClient
{
public:
	void StartClient();
	void RecieveString(SOCKET &socket, sockaddr_in &server);
};

#endif