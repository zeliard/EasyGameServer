#pragma once

enum ThreadType
{
	THREAD_MAIN = 1,
	THREAD_CLIENT = 2,
	THREAD_DATABASE = 3
};

typedef int SOCKET;

unsigned int ClientHandlingThread(void* lpParam);
unsigned int DatabaseHandlingThread(void* lpParam);
//void TimerProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue);
