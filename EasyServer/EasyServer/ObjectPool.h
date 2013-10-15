#pragma once

#include "Exception.h"

template <class TOBJECT, int ALLOC_COUNT=10>
class ObjectPool
{
public:

	static void* operator new(size_t objSize)
	{
		if (!mFreeList)
		{
			mFreeList = new uint8_t[sizeof(TOBJECT)*ALLOC_COUNT] ;

			uint8_t* pNext = mFreeList ;
			uint8_t** ppCurr = reinterpret_cast<uint8_t**>(mFreeList) ;

			for (int i=0 ; i<ALLOC_COUNT-1 ; ++i)
			{
				pNext += sizeof(TOBJECT) ;
				*ppCurr = pNext ;
				ppCurr = reinterpret_cast<uint8_t**>(pNext) ;
			}
			
			*ppCurr = nullptr ;
			mTotalAllocCount += ALLOC_COUNT ;
		}

		uint8_t* pAvailable = mFreeList ;
		mFreeList = *reinterpret_cast<uint8_t**>(pAvailable) ;
		++mCurrentUseCount ;

		return pAvailable ;
	}

	static void	operator delete(void* obj)
	{
		CRASH_ASSERT( mCurrentUseCount > 0 ) ;
	
		--mCurrentUseCount ;
	
		*reinterpret_cast<uint8_t**>(obj) = mFreeList ;
		mFreeList = static_cast<uint8_t*>(obj) ;
	}


private:
	static uint8_t*	mFreeList ;
	static int		mTotalAllocCount ;
	static int		mCurrentUseCount ;
	
	
};


template <class TOBJECT, int ALLOC_COUNT>
uint8_t* ObjectPool<TOBJECT, ALLOC_COUNT>::mFreeList = nullptr ;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::mTotalAllocCount = 0 ;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::mCurrentUseCount = 0 ;


