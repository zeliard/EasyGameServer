// TestClient.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "TestClient.h"

#include <winsock2.h>
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>


#include "..\..\PacketType.h"
#include "CircularBuffer.h"

#pragma comment(lib,"ws2_32.lib")

#define MAX_LOADSTRING 100

#define IDC_SEND_BUTTON	103
#define WM_SOCKET		104

#define BUFSIZE	1024*10


SOCKET g_Socket = NULL ;
CircularBuffer g_SendBuffer(BUFSIZE) ;
CircularBuffer g_RecvBuffer(BUFSIZE) ;

char* szServer = "localhost" ;
int nPort = 9001 ;

bool g_LoginComplete = false ;

/// 서버에서 받아온 나의 ID 
int g_MyClientId = -1 ; 

bool Initialize()
{
	WSADATA WsaDat ;

	int nResult = WSAStartup(MAKEWORD(2,2),&WsaDat) ;
	if ( nResult != 0 )
		return false ;

	g_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
	if ( g_Socket == INVALID_SOCKET )
		return false ;

	return true ;
}

bool Connect(const char* serverAddr, int port)
{
	// Resolve IP address for hostname
	struct hostent* host ;

	if ( (host=gethostbyname(serverAddr) ) == NULL )
		return false ;

	// Set up our socket address structure
	SOCKADDR_IN SockAddr ;
	SockAddr.sin_port = htons(port) ;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr) ;

	if ( SOCKET_ERROR == connect(g_Socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr)) )
	{
		if (GetLastError() != WSAEWOULDBLOCK )
			return false ;
	}
	

	return true ;
}

/// 패킷처리 
void ProcessPacket(HWND hWnd)
{
	while ( true )
	{
		PacketHeader header ;

		if ( false == g_RecvBuffer.Peek((char*)&header, sizeof(PacketHeader)) )
			break ;

		if (header.mSize > g_RecvBuffer.GetCurrentSize() )
			break ;

		switch ( header.mType )
		{
		case PKT_SC_LOGIN:
			{
				LoginResult recvData ;
				if ( g_RecvBuffer.Read((char*)&recvData, header.mSize) )
				{
					// 패킷처리
					if ( recvData.mPlayerId == -1  )
					{
						/// 여기 걸리면 로그인 실패다.
						ExitProcess(-1) ;
					}
					

					g_MyClientId = recvData.mPlayerId ;
					g_LoginComplete = true ;
				
					char buff[128] = {0, } ;
					sprintf_s(buff, "LOGIN SUCCESS ClientId[%d] Name[%s] POS(%.4f, %.4f, %.4f) \n", g_MyClientId, recvData.mName, recvData.mPosX, recvData.mPosY, recvData.mPosZ) ;

					static int ypos = 33 ;
					HDC hdc = GetDC(hWnd) ;
					TextOutA(hdc, 10, 33, buff, strlen(buff)) ;
					ReleaseDC(hWnd, hdc) ;

					/// 채팅 방송 패킷 보내는 타이머 돌리자.. 
					SetTimer(hWnd, 337, 100, NULL) ;
				
				}
				else
				{
					assert(false) ;
				}
			}
			break ;

		case PKT_SC_CHAT:
			{
				ChatBroadcastResult recvData ;
				if ( g_RecvBuffer.Read((char*)&recvData, header.mSize) )
				{
					/// 여기 걸리면 로그인 안된놈이 보낸거다
					assert( recvData.mPlayerId != -1  ) ;

					char buff[MAX_CHAT_LEN] = {0, } ;
					sprintf_s(buff, "CHAT from Player[%s]: %s \n", recvData.mName, recvData.mChat ) ;

					static int y2pos = 60 ;
					HDC hdc = GetDC(hWnd) ;
					TextOutA(hdc, 10, y2pos, buff, strlen(buff)) ;
					ReleaseDC(hWnd, hdc) ;
					y2pos += 15 ;
					if ( y2pos > 600 )
						y2pos = 60 ;
					
				}
				else
				{
					assert(false) ;
				}
			}
			break ;
		default:
			assert(false) ;
		}

	}
}


// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;


	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTCLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTCLIENT));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTCLIENT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TESTCLIENT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1000, 700, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{

		case WM_CREATE:
		{
			// Create a push button
			CreateWindow(L"BUTTON", L"CONNECT", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
						10,	10, 175, 23, hWnd, (HMENU)IDC_SEND_BUTTON, GetModuleHandle(NULL), NULL);

			if ( false == Initialize() )
			{
				SendMessage(hWnd,WM_DESTROY,NULL,NULL) ;
				break ;
			}
			
			int	nResult = WSAAsyncSelect(g_Socket, hWnd, WM_SOCKET,(FD_CLOSE|FD_CONNECT));
			if (nResult)
			{
				MessageBox(hWnd, L"WSAAsyncSelect failed", L"Critical Error", MB_ICONERROR);
				SendMessage(hWnd,WM_DESTROY,NULL,NULL);
				break;
			}
		
		}
		break;

		case WM_TIMER:
		{
			/// 주기적으로 채팅 날려보자.

			ChatBroadcastRequest sendData ;
			
			sendData.mPlayerId = g_MyClientId ;
			
			/// 랜덤 문자열을 채팅으로 날리기
			char* buff = sendData.mChat ; 
			for (int i=0 ; i<300 ; ++i )
			{
				sendData.mChat[i] = (char) (65 + (rand() % 26) ) ;
			}
			sendData.mChat[300] = '\0' ;
			

			if ( g_SendBuffer.Write((const char*)&sendData, sendData.mSize) )
			{
				PostMessage(hWnd, WM_SOCKET, wParam, FD_WRITE) ;
			}
			
			
		}
		break ;

		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// 메뉴 선택을 구문 분석합니다.

			switch (wmId)
			{
			case IDC_SEND_BUTTON:
			{
				if ( !g_LoginComplete && !Connect(szServer, nPort) )
				{
					SendMessage(hWnd,WM_DESTROY,NULL,NULL) ;
					break ;
				}

			}
			break;

			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;

		case WM_SOCKET:
		{
			if (WSAGETSELECTERROR(lParam))
			{	
				MessageBox(hWnd,L"WSAGETSELECTERROR",	L"Error", MB_OK|MB_ICONERROR);
				SendMessage(hWnd,WM_DESTROY,NULL,NULL);
				break;
			}

			switch (WSAGETSELECTEVENT(lParam))
			{
			case FD_CONNECT:
				{
					/// NAGLE 끈다
					int opt = 1 ;
					::setsockopt(g_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int)) ;

					srand( time(NULL) ) ;
					/// 대략 1000~1100 의 ID로 로그인 해보자 
					LoginRequest sendData ;
					sendData.mPlayerId = 1000 + rand() % 101 ;

					if ( g_SendBuffer.Write((const char*)&sendData, sendData.mSize) )
					{
						PostMessage(hWnd, WM_SOCKET, wParam, FD_WRITE) ;
					}


					int nResult = WSAAsyncSelect(g_Socket, hWnd, WM_SOCKET, (FD_CLOSE|FD_READ|FD_WRITE) ) ;
					if (nResult)
					{
						assert(false) ;
						break;
					}
				}
				break ;

			case FD_READ:
				{
					char inBuf[4096] = {0, } ;
					
					int recvLen = recv(g_Socket, inBuf, 4096, 0) ;

					if ( !g_RecvBuffer.Write(inBuf, recvLen) )
					{
						/// 버퍼 꽉찼다. 
						assert(false) ;
					}
					
					ProcessPacket(hWnd) ;
					
				}
				break;

			case FD_WRITE:
				{
					/// 실제로 버퍼에 있는것들 꺼내서 보내기
					int size = g_SendBuffer.GetCurrentSize() ;
					if ( size > 0 )
					{
						char* data = new char[size] ;
						g_SendBuffer.Peek(data) ;

						int sent = send(g_Socket, data, size, 0) ;
						
						/// 다를수 있다
						if ( sent != size )
							OutputDebugStringA("sent != request\n") ;

						g_SendBuffer.Consume(sent) ;

						delete [] data ;
					}
				
				}
				break ;

			case FD_CLOSE:
				{
					MessageBox(hWnd, L"Server closed connection", L"Connection closed!", MB_ICONINFORMATION|MB_OK);
					closesocket(g_Socket);
					SendMessage(hWnd,WM_DESTROY,NULL,NULL);
				}
				break;
			}
		} 
		break ;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

