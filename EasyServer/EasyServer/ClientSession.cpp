#include "stdafx.h"
#include "ClientSession.h"
#include "..\..\PacketType.h"
#include "ClientManager.h"
#include "DatabaseJobContext.h"
#include "DatabaseJobManager.h"


bool ClientSession::OnConnect(SOCKADDR_IN* addr)
{
	memcpy(&mClientAddr, addr, sizeof(SOCKADDR_IN)) ;

	/// 소켓을 넌블러킹으로 바꾸고
	u_long arg = 1 ;
	ioctlsocket(mSocket, FIONBIO, &arg) ;

	/// nagle 알고리즘 끄기
	int opt = 1 ;
	setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int)) ;

	printf("[DEBUG] Client Connected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port)) ;
	
	mConnected = true ;

	return PostRecv() ;
}

bool ClientSession::PostRecv()
{
	if ( !IsConnected() )
		return false ;

	DWORD recvbytes = 0 ;
	DWORD flags = 0 ;
	WSABUF buf ;
	buf.len = (ULONG)mRecvBuffer.GetFreeSpaceSize() ;
	buf.buf = (char*)mRecvBuffer.GetBuffer() ;

	memset(&mOverlappedRecv, 0, sizeof(OverlappedIO)) ;
	mOverlappedRecv.mObject = this ;

	/// 비동기 입출력 시작
	if ( SOCKET_ERROR == WSARecv(mSocket, &buf, 1, &recvbytes, &flags, &mOverlappedRecv, RecvCompletion) )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
			return false ;
	}

	IncOverlappedRequest() ;

	return true ;
}

void ClientSession::Disconnect()
{
	if ( !IsConnected() )
		return ;

	printf("[DEBUG] Client Disconnected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port)) ;

	/// 즉각 해제

	LINGER lingerOption;
	lingerOption.l_onoff = 1;
	lingerOption.l_linger = 0;

	/// no TCP TIME_WAIT
	if (SOCKET_ERROR == setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&lingerOption, sizeof(LINGER)))
	{
		printf_s("[DEBUG] setsockopt linger option error: %d\n", GetLastError());
		return;
	}

	closesocket(mSocket) ;

	mConnected = false ;
}




bool ClientSession::SendRequest(PacketHeader* pkt)
{
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
	if (!IsConnected())
		return false;

	/// 보낼 데이터가 없으면 그냥 리턴
	if (mSendBuffer.GetContiguiousBytes() == 0)
		return true;

	DWORD sendbytes = 0;
	DWORD flags = 0;

	WSABUF buf;
	buf.len = (ULONG)mSendBuffer.GetContiguiousBytes();
	buf.buf = (char*)mSendBuffer.GetBufferStart();

	memset(&mOverlappedSend, 0, sizeof(OverlappedIO));
	mOverlappedSend.mObject = this;

	// 비동기 입출력 시작
	if (SOCKET_ERROR == WSASend(mSocket, &buf, 1, &sendbytes, flags, &mOverlappedSend, SendCompletion))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
			return false;
	}

	IncOverlappedRequest();

	//assert(buf.len == sendbytes);

	return true;
}

void ClientSession::OnWriteComplete(size_t len)
{
	/// 보내기 완료한 데이터는 버퍼에서 제거
	mSendBuffer.Remove(len) ;
}

bool ClientSession::Broadcast(PacketHeader* pkt)
{
	if ( !SendRequest(pkt) )
		return false ;

	if ( !IsConnected() )
		return false ;

	GClientManager->BroadcastPacket(this, pkt) ;

	return true ;
}

void ClientSession::OnTick()
{
	/// 클라별로 주기적으로 해야될 일은 여기에

	/// 특정 주기로 DB에 위치 저장
	if ( ++mDbUpdateCount == PLAYER_DB_UPDATE_CYCLE )
	{
		mDbUpdateCount = 0 ;
		UpdatePlayerDataContext* updatePlayer = new UpdatePlayerDataContext(mSocket, mPlayerId) ;

		updatePlayer->mPosX = mPosX ;
		updatePlayer->mPosY = mPosY ;
		strcpy_s(updatePlayer->mComment, "updated_test") ; ///< 일단은 테스트를 위해
		GDatabaseJobManager->PushDatabaseJobRequest(updatePlayer) ;
	}
	
}

void ClientSession::DatabaseJobDone(DatabaseJobContext* result)
{
	CRASH_ASSERT( mSocket == result->mSockKey ) ;
	

	const type_info& typeInfo = typeid(*result) ;

	if ( typeInfo == typeid(LoadPlayerDataContext) )
	{
		LoadPlayerDataContext* login = dynamic_cast<LoadPlayerDataContext*>(result) ;

		LoginDone(login->mPlayerId, (float)login->mPosX, (float)login->mPosY, login->mPlayerName) ;
	
	}
	else if ( typeInfo == typeid(UpdatePlayerDataContext) )
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
	strcpy_s(mPlayerName, name) ;
	strcpy_s(outPacket.mName, name) ;

	SendRequest(&outPacket) ;

	mLogon = true ;
}


