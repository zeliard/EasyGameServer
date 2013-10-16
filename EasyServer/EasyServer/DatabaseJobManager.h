#pragma once


#include "SPSCQueue.h"

struct DatabaseJobContext ;

class DatabaseJobManager
{
public:
	DatabaseJobManager() {}

	void ExecuteDatabaseJobs() ;
	
	bool PushDatabaseJobRequest(DatabaseJobContext* jobContext) ;
	bool PopDatabaseJobResult(DatabaseJobContext*& jobContext) ;

private:
	enum  
	{
		MAX_DB_JOB = 127 
	} ;

	SPSCQueue<DatabaseJobContext*, MAX_DB_JOB>	mDbJobRequestQueue ;
	SPSCQueue<DatabaseJobContext*, MAX_DB_JOB>	mDbJobResultQueue ;
	
} ;

extern DatabaseJobManager* GDatabaseJobManager ;