#include <iostream>
#include <thread>


#include "Config.h"
#include "EasyGameServerLinux.h"
#include "ThreadLocal.h"
#include "ClientManager.h"
#include "DatabaseJobManager.h"
#include "DbHelper.h"

int main(int argc, char *argv[])
{
	LThreadType = THREAD_MAIN;

	/// Manager Init
	GClientManager = new ClientManager;
	GDatabaseJobManager = new DatabaseJobManager;

	/// DB Helper ÃÊ±âÈ­
	if (false == DbHelper::Initialize(DB_CONN_STR))
		return -1;

	/// bind and listen
	if (false == GClientManager->Initialize())
		return -1;
	

	std::thread clientThread(&ClientManager::EventLoop, GClientManager);
	std::thread dbTHread(&DatabaseJobManager::ExecuteDatabaseJobs, GDatabaseJobManager);

	printf("Server Started...\n");

	clientThread.join();
	dbTHread.join();

	DbHelper::Finalize();

	delete GClientManager;
	delete GDatabaseJobManager;

	return 0;
}