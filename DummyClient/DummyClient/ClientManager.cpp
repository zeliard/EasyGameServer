#include "stdafx.h"
#include "DummyClient.h"
#include "ClientSession.h"
#include "ClientManager.h"

void ClientManager::Start()
{
	tcp::endpoint serv(boost::asio::ip::address::from_string(CONNECT_ADDR), CONNECT_PORT);

	for (int i = 0; i < mTotalDummyCount; ++i)
	{
		boost::shared_ptr<ClientSession> newClient(new ClientSession(mIoService, START_PLAYER_ID + i));
		newClient->Connect(serv);
		mClientList.push_back(newClient);
	}

	mIoService.run();
}