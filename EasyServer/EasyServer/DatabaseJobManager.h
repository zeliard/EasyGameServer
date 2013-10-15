#pragma once


#include "SPSCQueue.h"

//TODO 포인터로 바꾸고 전방 선언
#include "DatabaseJobContext.h"

class DatabaseJobManager
{
public:
	DatabaseJobManager() {}

	bool ExecuteDatabaseJobs() ;
	
	bool PushDatabaseJobRequest(const DatabaseJobRequest& request) ;
	bool PopDatabaseJobResult(DatabaseJobResult& result) ;

private:
	enum  
	{
		MAX_DB_JOB = 1023 
	} ;

	SPSCQueue<DatabaseJobRequest,MAX_DB_JOB>	mDbJobRequestQueue ;
	SPSCQueue<DatabaseJobResult,MAX_DB_JOB>		mDbJobResultQueue ;
	
} ;

extern DatabaseJobManager* GDatabaseJobManager ;