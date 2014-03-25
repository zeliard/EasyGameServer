#include "stdafx.h"
#include "EasyServer.h"

#include "Config.h"
#include "..\..\PacketType.h"

#include "Exception.h"
#include "ClientSession.h"
#include "ClientManager.h"
#include "DatabaseJobManager.h"
#include "DbHelper.h"

#pragma comment(lib,"ws2_32.lib")


SOCKET g_AcceptedSocket = NULL ;
bool g_DidRegisterAcceptedSocket = false;

__declspec(thread) int LThreadType = -1 ;


int _tmain(int argc, _TCHAR* argv[])
{
	/// crash �߻��� dump ����� ���ؼ�
	SetUnhandledExceptionFilter(ExceptionFilter) ;

	LThreadType = THREAD_MAIN ;

	/// Manager Init
	GClientManager = new ClientManager ;
	GDatabaseJobManager = new DatabaseJobManager ;

	/// DB Helper �ʱ�ȭ
	if ( false == DbHelper::Initialize(DB_CONN_STR) )
		return -1 ;

	/// ���� �ʱ�ȭ
	WSADATA wsa ;
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1 ;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0) ;
	if (listenSocket == INVALID_SOCKET)
		return -1 ;

	int opt = 1 ;
	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int) ) ;

	/// bind
	SOCKADDR_IN serveraddr ;
	ZeroMemory(&serveraddr, sizeof(serveraddr)) ;
	serveraddr.sin_family = AF_INET ;
	serveraddr.sin_port = htons(LISTEN_PORT) ;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY) ;
	int ret = bind(listenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) ;
	if (ret == SOCKET_ERROR)
		return -1 ;
	
	/// listen
	ret = listen(listenSocket, SOMAXCONN) ;
	if (ret == SOCKET_ERROR)
		return -1 ;

	/// auto-reset event
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL) ;
	if (hEvent == NULL)
		return -1 ;

	/// Client Logic + I/O Thread
	DWORD dwThreadId ;
	HANDLE hThread = (HANDLE)_beginthreadex (NULL, 0, ClientHandlingThread, hEvent, 0, (unsigned int*)&dwThreadId) ;
    if (hThread == NULL)
		return -1 ;

	/// DB Thread
	HANDLE hDbThread = (HANDLE)_beginthreadex (NULL, 0, DatabaseHandlingThread, NULL, 0, (unsigned int*)&dwThreadId) ;
	if (hDbThread == NULL)
		return -1 ;

	/// accept loop
	while ( true )
	{
		if(g_DidRegisterAcceptedSocket == true)
			continue;
		g_AcceptedSocket = accept(listenSocket, NULL, NULL) ;
		if ( g_AcceptedSocket == INVALID_SOCKET )
		{
			printf("accept: invalid socket\n") ;
			continue ;
		}

		/// accept event fire!
		if ( !SetEvent(hEvent) )
		{
			g_DidRegisterAcceptedSocket = true;
			printf("SetEvent error: %d\n",GetLastError()) ;
			break ;
		}
	}

	CloseHandle( hThread ) ;
	CloseHandle( hEvent ) ;
	CloseHandle( hDbThread ) ;

	// ���� ����
	WSACleanup() ;

	DbHelper::Finalize() ;

	delete GClientManager ;
	delete GDatabaseJobManager ;
	return 0 ;
}

unsigned int WINAPI ClientHandlingThread( LPVOID lpParam )
{
	LThreadType = THREAD_CLIENT ;

	HANDLE hEvent = (HANDLE)lpParam ;

	/// Timer
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL) ;
	if (hTimer == NULL)
		return -1 ;

	LARGE_INTEGER liDueTime ;
	liDueTime.QuadPart = -10000000 ; ///< 1�� �ĺ��� ����
	if ( !SetWaitableTimer(hTimer, &liDueTime, 10, TimerProc, NULL, TRUE) )
		return -1 ;
		
	while ( true )
	{
		/// accept or IO/Timer completion   ���
		DWORD result = WaitForSingleObjectEx(hEvent, INFINITE, TRUE) ;

		/// client connected
		if ( result == WAIT_OBJECT_0 )
		{
			/// ���� ���� ����ü �Ҵ�� �ʱ�ȭ
			ClientSession* client = GClientManager->CreateClient(g_AcceptedSocket) ;
			
			SOCKADDR_IN clientaddr ;
			int addrlen = sizeof(clientaddr) ;
			getpeername(g_AcceptedSocket, (SOCKADDR*)&clientaddr, &addrlen) ;

			// Ŭ�� ���� ó��
			if ( false == client->OnConnect(&clientaddr) )
			{
				client->Disconnect() ;
			}

			g_DidRegisterAcceptedSocket = false;
		
			continue ; ///< �ٽ� ����
		}

		// APC�� �ִ� completion�� �ƴ϶�� ������
		if ( result != WAIT_IO_COMPLETION )
			return -1 ;
	}

	CloseHandle( hTimer ) ;
	return 0;
} 

unsigned int WINAPI DatabaseHandlingThread( LPVOID lpParam )
{
	LThreadType = THREAD_DATABASE ;

	GDatabaseJobManager->ExecuteDatabaseJobs() ;
	
	return 0 ;
}

void CALLBACK TimerProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	assert( LThreadType == THREAD_CLIENT ) ;

	GClientManager->OnPeriodWork() ;
}
