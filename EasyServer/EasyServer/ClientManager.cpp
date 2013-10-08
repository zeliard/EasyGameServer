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

void ClientManager::OnPeriodWork()
{
	/// 접속이 끊긴 세션들 주기적으로 정리 (1초 정도 마다 해주자)
	DWORD currTick = GetTickCount() ;
	if ( currTick - mLastGCTick >= 1000 )
	{
		CollectGarbageSessions() ;
		mLastGCTick = currTick ;
	}
		
}

void ClientManager::CollectGarbageSessions()
{
	std::vector<ClientSession*> disconnectedSessions ;

	for (auto it=mClientList.begin() ; it!=mClientList.end() ; ++it)
	{
		ClientSession* client = it->second ;

		if ( false == client->IsConnected() )
			disconnectedSessions.push_back(client) ;
	}

	for (size_t i=0 ; i<disconnectedSessions.size() ; ++i)
	{
		ClientSession* client = disconnectedSessions[i] ;
		mClientList.erase(client->mSocket) ;
		delete client ;
	}

}