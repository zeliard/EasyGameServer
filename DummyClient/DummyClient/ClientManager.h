#pragma once

#include <vector>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

class ClientSession; 

class ClientManager
{
public:
	ClientManager(boost::asio::io_service& io_service, int total=1) 
		: mIoService(io_service), mTotalDummyCount(total)
	{}



	void Start();
	

private:
	boost::asio::io_service& mIoService;

	std::vector<boost::shared_ptr<ClientSession>> mClientList;

	int mTotalDummyCount;

};