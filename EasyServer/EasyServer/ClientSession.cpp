#include "stdafx.h"
#include "ClientSession.h"
#include "..\..\PacketType.h"
#include "ClientManager.h"


bool ClientSession::OnConnect(SOCKADDR_IN* addr)
{
	memcpy(&mClientAddr, addr, sizeof(SOCKADDR_IN)) ;

	/// 소켓을 넌블러킹으로 바꾸고
	u_long arg = 1 ;
	::ioctlsocket(mSocket, FIONBIO, &arg) ;

	/// nagle 알고리즘 끄기
	int opt = 1 ;
	::setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int)) ;

	printf("[DEBUG] Client Connected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port)) ;


	bool ok = PostRecv() ;

	if ( ok )
	{
		/// 여기에 클라에게 접속시 뭐 통보할 것 있으면 넣는다

	}

	return ok ;
}

bool ClientSession::PostRecv()
{
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

	return true ;
}

void ClientSession::Disconnect()
{

	printf("[DEBUG] Client Disconnected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port)) ;

	::shutdown(mSocket, SD_BOTH) ;
	::closesocket(mSocket) ;
	
}


void ClientSession::OnRead(size_t len)
{
	mRecvBuffer.Commit(len) ;

	/// 패킷 파싱하고 처리
	while ( true )
	{
		/// 패킷 헤더 크기 만큼 읽어와보기
		PacketHeader header ;
		if ( false == mRecvBuffer.Peek((char*)&header, sizeof(PacketHeader)) )
			return ;

		/// 패킷 완성이 되는가? 
		if ( mRecvBuffer.GetStoredSize() < (header.mSize - sizeof(PacketHeader)) )
			return ;

		/// 패킷 핸들링
		switch ( header.mType )
		{
		case PKT_CS_PING:
			{
				TestPing inPacket ;
				mRecvBuffer.Read((char*)&inPacket, header.mSize) ;
				
				TestPong outPacket ;
				sprintf_s(outPacket.mData, "[REPLY] CS_PING: %d, %f, %f, %f \n", inPacket.mPlayerId, inPacket.mPosX, inPacket.mPosY, inPacket.mPosZ ) ;
				outPacket.mResult = true ;
				outPacket.mPlayerId = inPacket.mPlayerId + 10000 ;
				
				if ( !Broadcast(&outPacket) )
				{
					GClientManager->DeleteClient(this) ;
					return ;
				}
	
			}
			break ;
		case PKT_TEST:
			{
				TestEchoPacket inPacket ;
				mRecvBuffer.Read((char*)&inPacket, header.mSize) ;

				printf("[DEBUG] PKT_TEST: %d, %s \n", inPacket.mPlayerId, inPacket.mData ) ;
 
			}
			break ;

		default:
			{
				/// 여기 들어오면 이상한 패킷 보낸거다.
				Disconnect() ;
				GClientManager->DeleteClient(this) ;
				return ;
			}
			break ;
		}
	}
}

bool ClientSession::Send(PacketHeader* pkt)
{
	
	/// 버퍼 용량 부족인 경우는 끊어버림
	if ( false == mSendBuffer.Write((char*)pkt, pkt->mSize) )
	{
		Disconnect() ;
		return false ;
	}

	/// 보낼 데이터가 있는지 검사
	if ( mSendBuffer.GetContiguiousBytes() == 0 )
	{
		/// 방금전에 write 했는데, 데이터가 없다면 뭔가 잘못된 것
		assert(false) ;
		Disconnect() ;
		return false ;
	}
		
	DWORD sendbytes = 0 ;
	DWORD flags = 0 ;

	WSABUF buf ;
	buf.len = (ULONG)mSendBuffer.GetContiguiousBytes() ;
	buf.buf = (char*)mSendBuffer.GetBufferStart() ;
	
	memset(&mOverlappedSend, 0, sizeof(OverlappedIO)) ;
	mOverlappedSend.mObject = this ;

	// 비동기 입출력 시작
	if ( SOCKET_ERROR == WSASend(mSocket, &buf, 1, &sendbytes, flags, &mOverlappedSend, SendCompletion) )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
			return false ;
	}

	return true ;
}

void ClientSession::OnWriteComplete(size_t len)
{
	/// 보내기 완료한 데이터는 버퍼에서 제거
	mSendBuffer.Remove(len) ;

	/// 얼래? 덜 보낸 경우도 있나? (커널의 send queue가 꽉찼거나, Send Completion이전에 또 send 한 경우?)
	if ( mSendBuffer.GetContiguiousBytes() > 0 )
	{
		assert(false) ;
	}

}

bool ClientSession::Broadcast(PacketHeader* pkt)
{
	if ( !Send(pkt) )
		return false ;

	GClientManager->BroadcastPacket(this, pkt) ;

	return true ;
}

///////////////////////////////////////////////////////////

void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	ClientSession* fromClient = static_cast<OverlappedIO*>(lpOverlapped)->mObject ;

	/// 에러 발생시 해당 세션 종료
	if ( dwError || cbTransferred == 0 )
	{
		fromClient->Disconnect() ;
		GClientManager->DeleteClient(fromClient) ;
		return ;
	}

	/// 받은 데이터 처리
	fromClient->OnRead(cbTransferred) ;

	/// 다시 받기
	if ( false == fromClient->PostRecv() )
	{
		fromClient->Disconnect() ;
		GClientManager->DeleteClient(fromClient) ;
		return ;
	}
}


void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	ClientSession* fromClient = static_cast<OverlappedIO*>(lpOverlapped)->mObject ;

	/// 에러 발생시 해당 세션 종료
	if ( dwError || cbTransferred == 0 )
	{
		fromClient->Disconnect() ;
		GClientManager->DeleteClient(fromClient) ;
		return ;
	}

	fromClient->OnWriteComplete(cbTransferred) ;

}

