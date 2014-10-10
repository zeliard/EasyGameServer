#pragma once

#include <stdint.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "../../PacketType.h"

using boost::asio::ip::tcp;

#define RECV_BUF_SIZE 4096


class ClientSession : public boost::enable_shared_from_this < ClientSession >
{
public:
	ClientSession(boost::asio::io_service& io_service, int pid) 
		: mSocket(io_service), mPlayerId(pid)
	{}

	virtual ~ClientSession()
	{}


	void Connect(tcp::endpoint& target)
	{
		mSocket.async_connect(target, boost::bind(&ClientSession::HandleConnect, shared_from_this(), boost::asio::placeholders::error));
	}

	void HandleConnect(const boost::system::error_code& error)
	{
		PostReadHeader();

		LoginRequest pktLogin;
		pktLogin.mPlayerId = mPlayerId;
		SendPacket(pktLogin);

	}

	void PostReadHeader()
	{
		mReadBuf.fill(0);

		boost::asio::async_read(mSocket, boost::asio::buffer(mReadBuf, sizeof(PacketHeader)),
			boost::bind(&ClientSession::HandleReadHeader, shared_from_this(), boost::asio::placeholders::error)
			);
	}

	void HandleReadHeader(const boost::system::error_code& error)
	{
		if (!error)
		{
			int pktLen = static_cast<int>(mReadBuf[1]) << 8 | mReadBuf[0];

			PostReadPayload(pktLen - sizeof(PacketHeader));
		}
	}

	void PostReadPayload(int len)
	{
		boost::asio::mutable_buffers_1 mbuf = boost::asio::buffer(&mReadBuf[sizeof(PacketHeader)], len);

		boost::asio::async_read(mSocket, mbuf, 
			boost::bind(&ClientSession::HandleReadPayload, shared_from_this(), boost::asio::placeholders::error)
			);
	}

	void HandleReadPayload(const boost::system::error_code& error);

	template <class PKT_TYPE>
	void SendPacket(PKT_TYPE& pkt)
	{
		/// 동기전송
		boost::asio::write(mSocket, boost::asio::buffer((char*)&pkt, pkt.mSize));
	}




private:
	tcp::socket mSocket;
	std::array<uint8_t, RECV_BUF_SIZE> mReadBuf;

	int mPlayerId;

};


typedef boost::shared_ptr<ClientSession> ClientSessionPtr;
