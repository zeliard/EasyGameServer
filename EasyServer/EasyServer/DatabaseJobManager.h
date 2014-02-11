#pragma once


#include "ProducerConsumerQueue.h"

struct DatabaseJobContext ;

class DatabaseJobManager
{
public:
	DatabaseJobManager() {}

	void ExecuteDatabaseJobs() ;
	
	void PushDatabaseJobRequest(DatabaseJobContext* jobContext) ;
	bool PopDatabaseJobResult(DatabaseJobContext*& jobContext) ;

private:
	enum  
	{
		MAX_DB_JOB = 127 
	} ;

	ProducerConsumerQueue<DatabaseJobContext*, MAX_DB_JOB>	mDbJobRequestQueue;
	ProducerConsumerQueue<DatabaseJobContext*, MAX_DB_JOB>	mDbJobResultQueue;
	
} ;

extern DatabaseJobManager* GDatabaseJobManager ;