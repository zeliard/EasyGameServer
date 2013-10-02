#pragma once

#include <map>
#include <WinSock2.h>

class ClientSession ;
struct PacketHeader ;

class ClientManager
{
public:
	ClientManager() : mClientIndex(0)
	{}

	ClientSession* CreateClient(SOCKET sock) ;
	void DeleteClient(ClientSession* client) ;

	void BroadcastPacket(PacketHeader* pkt) ;

private:
	typedef std::map<SOCKET, ClientSession*> ClientList ;
	ClientList	mClientList ;
	int			mClientIndex ;
} ;

extern ClientManager* GClientManager ;