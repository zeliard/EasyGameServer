#include "stdafx.h"
#include "ClientSession.h"

void ClientSession::HandleReadPayload(const boost::system::error_code& error)
{
	if (error)
	{
		std::cout << error.message() << std::endl;
		return;
	}

	int pktLen = static_cast<int>(mReadBuf[1]) << 8 | mReadBuf[0];
	int pktType = static_cast<int>(mReadBuf[3]) << 8 | mReadBuf[2];

	switch (pktType)
	{
	case PKT_SC_LOGIN:
	{
		LoginResult pktResult;
		memcpy_s(&pktResult, pktLen, (void*)&mReadBuf, pktLen);

		std::cout << pktResult.mName << std::endl;
	}

		break;

	case PKT_SC_CHAT:
		break;
	case PKT_SC_MOVE:
		break;

	}

	PostReadHeader();


}