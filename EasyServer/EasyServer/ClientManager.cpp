#include "stdafx.h"
#include "..\..\PacketType.h"
#include "ClientSession.h"
#include "ClientManager.h"

ClientManager* GClientManager = NULL ;

ClientSession* ClientManager::CreateClient(SOCKET sock)
{
	if ( ++mClientIndex == INT_MAX )
		mClientIndex= 0 ;
	
	ClientSession* client = new ClientSession(sock, mClientIndex) ;
	mClientList.insert(ClientList::value_type(sock, client)) ;

	return client ;
}

void ClientManager::DeleteClient(ClientSession* client)
{
	mClientList.erase(client->mSocket) ;
	delete client ;
}

void ClientManager::BroadcastPacket(ClientSession* from, PacketHeader* pkt)
{

	for (auto it=mClientList.begin() ; it!=mClientList.end() ; ++it)
	{
		ClientSession* client = it->second ;
		
		if ( from == client )
			continue ;
		
		client->Send(pkt) ;
	}
}