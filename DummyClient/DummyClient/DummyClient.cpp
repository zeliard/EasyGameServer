// DummyClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DummyClient.h"
#include "ClientSession.h"
#include "ClientManager.h"
#include <boost/make_unique.hpp>

/// config values
int MAX_CONNECTION = 0;
char CONNECT_ADDR[32] = { 0, };
unsigned short CONNECT_PORT = 0;

char* GetCommandOption(char** begin, char** end, const std::string& comparand)
{
	char** itr = std::find(begin, end, comparand);
	if (itr != end && ++itr != end)
		return *itr;

	return nullptr;
}

///----------------------------------------------------------------------


int main(int argc, char* argv[])
{
	MAX_CONNECTION = 100;
	strcpy_s(CONNECT_ADDR, "127.0.0.1");
	CONNECT_PORT = 9001;

	if (argc < 2)
	{
		printf_s("Usage Example: DummyClients --ip 127.0.0.1 --port 9001 --session 100\n");
	}
	else
	{
		char* ipAddr = GetCommandOption(argv, argv + argc, "--ip");
		char* port = GetCommandOption(argv, argv + argc, "--port");
		char* session = GetCommandOption(argv, argv + argc, "--session");

		if (ipAddr)
			strcpy_s(CONNECT_ADDR, ipAddr);

		if (port)
			CONNECT_PORT = atoi(port);

		if (session)
			MAX_CONNECTION = atoi(session);

	}

	boost::asio::io_service io_service;

	try
	{
		auto mgr = boost::make_unique<ClientManager>(io_service, MAX_CONNECTION);

		mgr->Start(); ///< block here
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}


	return 0;
}

