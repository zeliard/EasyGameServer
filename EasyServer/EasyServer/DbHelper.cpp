#include "stdafx.h"
#include "DbHelper.h"
#include "sqlite3.h"
#include "Exception.h"

sqlite3* DbHelper::mSqlite = NULL ;


DbHelper::DbHelper(const char* sql) 
	: mResult(NULL), mResultColCount(0), mBindColCount(0), mResultCurrentCol(0)
{
	char* errMsg = NULL ;

	if ( sqlite3_prepare_v2(mSqlite, sql, -1, &mResult, NULL) != SQLITE_OK )
	{
		printf("DbHelper Query [%s] Prepare failed: %s\n", sql, sqlite3_errmsg(mSqlite)) ;
		CRASH_ASSERT(false) ;
	}
}

DbHelper::~DbHelper()
{
	sqlite3_finalize(mResult) ;
}

bool DbHelper::Initialize(const char* connInfoStr)
{
	int result = sqlite3_open(connInfoStr, &mSqlite) ;

	if ( mSqlite == NULL || result != SQLITE_OK )
	{
		printf("DbHelper::Initialize Error\n") ;
		return false ;
	}

	return true ;
}


void DbHelper::Finalize()
{
	if (mSqlite)
		sqlite3_close(mSqlite) ;
}

bool DbHelper::Execute(const char* format, ...)
{
	if (!format)
		return false ;

	if (!mSqlite)
		return false ;

	va_list ap ;
	char sqlQuery[1024] = {0, } ;
	va_start(ap, format) ;
	int res = vsnprintf_s(sqlQuery, 1024, format, ap) ; 
	va_end(ap) ;

	if (-1 == res)
		return false ;
	
	char* errMsg = NULL ;
	
	///FYI: 사실 실무에서는 (SQL Injection 때문에) 이렇게 쿼리를 직접 넣지 않고
	///파라미터별로 일일이 BIND한다. (BindParamXXX 멤버함수 참고)
	if (sqlite3_exec(mSqlite, sqlQuery, NULL, NULL, &errMsg) != SQLITE_OK)
	{
		printf("SQL [%s] ERROR [%s] \n", sqlQuery, errMsg) ;
		return false ;
	}


	return true ;
}


bool DbHelper::BindParamInt(int param)
{

	if ( sqlite3_bind_int(mResult, ++mBindColCount, param) != SQLITE_OK )
	{
		printf("DbHelper Bind Int failed: %s\n", sqlite3_errmsg(mSqlite)) ;
		return false ;
	}

	return true ;
}

bool DbHelper::BindParamDouble(double param)
{
	if ( sqlite3_bind_double(mResult, ++mBindColCount, param) != SQLITE_OK )
	{
		printf("DbHelper Bind Double failed: %s\n", sqlite3_errmsg(mSqlite)) ;
		return false ;
	}

	return true ;
}

bool DbHelper::BindParamText(const char* text, int count)
{
	if ( sqlite3_bind_text(mResult, ++mBindColCount, text, strlen(text), NULL) != SQLITE_OK )
	{
		printf("DbHelper Bind Text failed: %s\n", sqlite3_errmsg(mSqlite)) ;
		return false ;
	}

	return true ;
}

RESULT_TYPE DbHelper::FetchRow()
{
	int result = sqlite3_step(mResult) ;
	if ( result != SQLITE_ROW && result != SQLITE_DONE )
	{
		printf("DbHelper FetchRow failed: %s\n", sqlite3_errmsg(mSqlite)) ;
		return RESULT_ERROR ;
	}

	/// 결과셋으로 얻어올 데이터가 없다. (그냥 쿼리 실행만 된 것)
	if ( result == SQLITE_DONE )
		return RESULT_DONE ;

	mResultColCount = sqlite3_column_count(mResult) ;
	mResultCurrentCol = 0 ;

	return RESULT_ROW ;
}

int DbHelper::GetResultParamInt()
{
	CRASH_ASSERT( mResultCurrentCol < mResultColCount ) ;
	
	return sqlite3_column_int(mResult, mResultCurrentCol++) ;
}

double DbHelper::GetResultParamDouble()
{
	CRASH_ASSERT( mResultCurrentCol < mResultColCount ) ;

	return sqlite3_column_double(mResult, mResultCurrentCol++) ;
}

const unsigned char* DbHelper::GetResultParamText()
{
	CRASH_ASSERT( mResultCurrentCol < mResultColCount ) ;
	
	return sqlite3_column_text(mResult, mResultCurrentCol++) ;
}