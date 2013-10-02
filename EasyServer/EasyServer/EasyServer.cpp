// EasyServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "..\..\PacketType.h"
#include "ClientSession.h"
#include "ClientManager.h"

#pragma comment(lib,"ws2_32.lib")


SOCKET g_AcceptedSocket = NULL ;

unsigned int WINAPI ClientHandlingThread( LPVOID lpParam ) ;

int _tmain(int argc, _TCHAR* argv[])
{
	/// Manager Init
	GClientManager = new ClientManager ;

	/// ���� �ʱ�ȭ
	WSADATA wsa ;
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1 ;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0) ;
	if (listenSocket == INVALID_SOCKET)
		return -1 ;

	int opt = 1 ;
	::setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int) ) ;

	/// bind
	SOCKADDR_IN serveraddr ;
	ZeroMemory(&serveraddr, sizeof(serveraddr)) ;
	serveraddr.sin_family = AF_INET ;
	serveraddr.sin_port = htons(9001) ;
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

	/// I/O Thread
	DWORD dwThreadId ;
	HANDLE hThread = (HANDLE)_beginthreadex (NULL, 0, ClientHandlingThread, hEvent, 0, (unsigned int*)&dwThreadId) ;
    if (hThread == NULL)
		return -1 ;
	    
	/// accept loop
	while ( true )
	{
		g_AcceptedSocket = accept(listenSocket, NULL, NULL) ;
		if ( g_AcceptedSocket == INVALID_SOCKET )
		{
			wprintf(L"accept: invalid socket\n") ;
			continue ;
		}

		/// accept event fire!
		if ( !SetEvent(hEvent) )
		{
			wprintf(L"SetEvent error: %d\n",GetLastError()) ;
			break ;
		}
	}

	CloseHandle( hThread );

	// ���� ����
	WSACleanup() ;

	delete GClientManager ;
	return 0 ;
}

unsigned int WINAPI ClientHandlingThread( LPVOID lpParam )
{ 
	HANDLE hEvent = (HANDLE)lpParam ;
	while ( true )
	{
		/// accept or completion ���
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
				GClientManager->DeleteClient(client) ;
			}
		
			continue ; ///< �ٽ� ����
		}

		// APC�� �ִ� completion�� �ƴ϶�� ������
		if ( result != WAIT_IO_COMPLETION )
			return -1 ;
	}

	return 0;
} 
