#include <thread>
#include <chrono>

#include "EasyGameServerLinux.h"
#include "Exception.h"
#include "ThreadLocal.h"
#include "DatabaseJobContext.h"
#include "DatabaseJobManager.h"
#include "DbHelper.h"

DatabaseJobManager* GDatabaseJobManager = nullptr ;


void DatabaseJobManager::ExecuteDatabaseJobs()
{
	LThreadType = THREAD_DATABASE ;

	DatabaseJobContext* jobContext = nullptr ;
	while ( true )
	{
		if (mDbJobRequestQueue.PopFront(jobContext))
		{
			/// 여기서 DB호출해서 처리하고 
			jobContext->mSuccess = jobContext->OnExecute();

			/// 그 결과를 result queue에 담아 놓음
			CRASH_ASSERT(mDbJobResultQueue.PushBack(jobContext));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

/// 아래 함수는 클라이언트 처리 쓰레드에서 불려야 한다
void DatabaseJobManager::PushDatabaseJobRequest(DatabaseJobContext* jobContext)
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);
	CRASH_ASSERT( mDbJobRequestQueue.PushBack(jobContext) );
}

/// 아래 함수는 클라이언트 처리 쓰레드에서 불려야 한다
bool DatabaseJobManager::PopDatabaseJobResult(DatabaseJobContext*& jobContext)
{
	CRASH_ASSERT(LThreadType == THREAD_CLIENT);

	/// DB 작업 완료는 기다리지 않는다
	return mDbJobResultQueue.PopFront(jobContext) ;
}