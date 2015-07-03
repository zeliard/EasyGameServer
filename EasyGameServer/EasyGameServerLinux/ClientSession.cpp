#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <typeinfo>

#include "Scheduler.h"
#include "ClientSession.h"
#include "PacketType.h"
#include "ClientManager.h"
#include "DatabaseJobContext.h"
#include "DatabaseJobManager.h"

ClientSession::~ClientSession()
{
	/// 소켓 닫으면 EPOLL이 관리하는 FD에서도 자동으로 빼줌 
	close(mSocket);
}

void ClientSession::OnConnect(sockaddr_in* addr)
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	memcpy(&mClientAddr, addr, sizeof(sockaddr_in));

	/// 소켓을 넌블러킹으로 바꾸고
	int flag = fcntl(mSocket, F_GETFL, 0);
	fcntl(mSocket, F_SETFL, flag | O_NONBLOCK);

	/// nagle 알고리즘 끄기
	int opt = 1 ;
	setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int)) ;

	printf("[DEBUG] Client Connected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port)) ;
	
	mConnected = true ;

	IncRefCount();
}

void ClientSession::Disconnect()
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	if (!IsConnected())
		return;
	
	/// 즉각 해제
	linger lingerOption;
	lingerOption.l_onoff = 1;
	lingerOption.l_linger = 0;

	/// no TCP TIME_WAIT
	if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&lingerOption, sizeof(linger)) < 0)
	{
		printf("[DEBUG] setsockopt linger option error\n");
		return;
	}

	mConnected = false;

	printf("[DEBUG] Client Disconnected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port));

	DecRefCount();
}

void ClientSession::OnRelease()
{
	CRASH_ASSERT(!IsConnected());

	GClientManager->DeleteClient(this);
}

void ClientSession::OnReceive()
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	CRASH_ASSERT(IsConnected());

	while (true)
	{
		int nread = read(mSocket, mRecvBuffer.GetBuffer(), mRecvBuffer.GetFreeSpaceSize());
		if (nread < 0)
		{
			/// 더이상 읽어올 것이 없는 경우
			if (errno == EAGAIN)
				break;

			/// 에러가 생긴 경우
			Disconnect();
			return;
		}

		mRecvBuffer.Commit(nread);
		DispatchPacket();
	}
}


bool ClientSession::SendRequest(PacketHeader* pkt)
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	if ( !IsConnected() )
		return false ;

	/// Send 요청은 버퍼에 쌓아놨다가 한번에 보낸다.
	if ( false == mSendBuffer.Write((char*)pkt, pkt->mSize) )
	{
		/// 버퍼 용량 부족인 경우는 끊어버림
		Disconnect() ;
		return false ;
	}

	return true;

}

bool ClientSession::SendFlush()
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	if (!IsConnected())
		return false;

	/// 보낼 데이터가 없으면 그냥 리턴
	if (mSendBuffer.GetContiguiousBytes() == 0)
		return true;

	while (mSendBuffer.GetContiguiousBytes() > 0)
	{
		int sent = write(mSocket, mSendBuffer.GetBufferStart(), mSendBuffer.GetContiguiousBytes());
		if (sent < 0)
		{
			if (errno == EAGAIN)
				return true;

			return false;
		}

		/// 보내기 완료한 데이터는 버퍼에서 제거
		mSendBuffer.Remove(sent);
	}

	return true;
}


bool ClientSession::Broadcast(PacketHeader* pkt)
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	if ( !SendRequest(pkt) )
		return false ;

	if ( !IsConnected() )
		return false ;

	GClientManager->BroadcastPacket(this, pkt) ;

	return true ;
}

void ClientSession::OnTick()
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	if (!IsConnected())
		return;

	/// 클라별로 주기적으로 해야될 일은 여기에


	
	CallFuncAfter(PLAYER_HEART_BEAT, this, &ClientSession::OnTick);
}

void ClientSession::OnDbUpdate()
{
	if (!IsConnected())
		return;

	UpdatePlayerDataContext* updatePlayer = new UpdatePlayerDataContext(mSocket, mPlayerId) ;
	 
	updatePlayer->mPosX = mPosX ;
	updatePlayer->mPosY = mPosY ;
	strcpy(updatePlayer->mComment, "updated_test") ; ///< 일단은 테스트를 위한 코멘트
	GDatabaseJobManager->PushDatabaseJobRequest(updatePlayer) ;

	CallFuncAfter(PLAYER_DB_UPDATE_INTERVAL, this, &ClientSession::OnDbUpdate);

}


void ClientSession::DatabaseJobDone(DatabaseJobContext* result)
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);
	CRASH_ASSERT( mSocket == result->mSockKey ) ;

	if (typeid(*result) == typeid(LoadPlayerDataContext))
	{
		LoadPlayerDataContext* login = dynamic_cast<LoadPlayerDataContext*>(result) ;

		LoginDone(login->mPlayerId, (float)login->mPosX, (float)login->mPosY, login->mPlayerName) ;
	
	}
	else if (typeid(*result) == typeid(UpdatePlayerDataContext))
	{
		UpdateDone() ;
	}
	else
	{
		CRASH_ASSERT(false) ;
	}

}

void ClientSession::UpdateDone()
{
	/// 콘텐츠를 넣기 전까지는 딱히 해줄 것이 없다. 단지 테스트를 위해서..
	printf("Player[%d] Update Done\n", mPlayerId) ;
}



void ClientSession::LoginDone(int pid, float x, float y, const char* name)
{
	LoginResult outPacket ;

	outPacket.mPlayerId = mPlayerId = pid ;
	outPacket.mPosX = mPosX = x ;
	outPacket.mPosY = mPosY = y ;
	strcpy(mPlayerName, name) ;
	strcpy(outPacket.mName, name) ;

	SendRequest(&outPacket) ;

	mLogon = true ;

	/// heartbeat gogo
	OnTick();

	/// first db update gogo
	OnDbUpdate();
}


