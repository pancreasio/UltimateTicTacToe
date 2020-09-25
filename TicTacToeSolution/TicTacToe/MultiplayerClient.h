#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <WS2tcpip.h>
#include <string>

class MultiplayerClient
{
	void StartClient();
	void RecieveString(SOCKET &socket, sockaddr_in &server);
};

#endif
