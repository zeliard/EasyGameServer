#include "stdafx.h"
#include "DatabaseJobContext.h"
#include "SQLStatement.h"
#include "DbHelper.h"



bool LoadPlayerDataContext::OnExecute()
{
	DbHelper dbhelper(SQL_SelectTest) ;
	dbhelper.BindParamInt(mPlayerId) ;

	/// 데이터가 없네?
	if ( RESULT_ROW != dbhelper.FetchRow() )
		return false ;


	const unsigned char* name = dbhelper.GetResultParamText() ;
	mPosX = dbhelper.GetResultParamDouble() ;
	mPosY = dbhelper.GetResultParamDouble() ;
	mPosZ = dbhelper.GetResultParamDouble() ;

	strcpy_s(mPlayerName, (char*)name) ;


	return true ;
}



	/* TEST

	static int testid = 1000 ;
	char testbuf[16] ; 
	sprintf_s(testbuf, "player%d", testid) ;
	DbHelper dbhelper(SQL_InsertTest) ;
	dbhelper.BindParamInt(testid) ;
	dbhelper.BindParamText(testbuf, strlen(testbuf)) ;
	dbhelper.BindParamDouble(rand()) ;
	dbhelper.BindParamDouble(rand()) ;
	dbhelper.BindParamDouble(rand()) ;
	dbhelper.BindParamText(testbuf, strlen(testbuf)) ;

	dbhelper.FetchRow() ;

	testid++ ;
	
	*/

	/*
	static int testid = 1000 ;



	testid++ ;
	

	

	char testbuf[20] ; 
	sprintf_s(testbuf, "abc_comment_TEST") ;

	DbHelper dbh2(SQL_UpdateTest) ;
	dbh2.BindParamText(testbuf, strlen(testbuf)) ;
	dbh2.BindParamInt(1238) ;
	if ( RESULT_DONE != dbh2.FetchRow() )
	{
		printf("error.........\n") ;
	}
	*/

	/*
	DbHelper dbh3(SQL_DeleteTest) ;
	dbh3.BindParamInt(1237) ;
	dbh3.FetchRow() ;
	*/