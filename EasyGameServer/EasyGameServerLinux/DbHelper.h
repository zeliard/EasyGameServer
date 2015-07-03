#pragma once

struct sqlite3 ;
struct sqlite3_stmt ;

enum RESULT_TYPE
{
	RESULT_ERROR,
	RESULT_ROW,
	RESULT_DONE
} ;

class DbHelper
{
public:
	DbHelper(const char* sql) ;
	~DbHelper() ;

	static bool Initialize(const char* connInfoStr) ;
	static void Finalize() ;

	static bool Execute(const char* format, ...) ;
	
	/// 아래는 DbHelper 객체를 선언하고 사용해야 함

	bool BindParamInt(int param) ;
	bool BindParamDouble(double param) ;
	bool BindParamText(const char* text, int count) ;

	RESULT_TYPE FetchRow() ;

	/// FetchRow시 데이터가 있다면 반드시 차례대로 호출해야함
	int GetResultParamInt() ;
	double GetResultParamDouble() ;
	const unsigned char* GetResultParamText() ;


private:

	static sqlite3* mSqlite ;

	/// 현재 수행중인 SQL문에 대한 정보
	sqlite3_stmt*	mResult ;
	int				mResultColCount ;

	int				mBindColCount ;
	int				mResultCurrentCol ;

} ;