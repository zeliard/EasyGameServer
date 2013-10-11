#pragma once

enum ThreadType
{
	THREAD_MAIN		= 1,
	THREAD_CLIENT	= 2,
	THREAD_DATABASE = 3
} ;

extern __declspec(thread) int LThreadType ;
