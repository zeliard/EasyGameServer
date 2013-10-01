#include "stdafx.h"
#include "CircularBuffer.h"


CircularBuffer::CircularBuffer(size_t capacity)
  : mBeginIndex(0), mEndIndex(0), mCurrentSize(0), mCapacity(capacity)
{
	mData = new char[capacity] ;
}

CircularBuffer::~CircularBuffer()
{
	delete [] mData ;
}

bool CircularBuffer::Write(const char* data, size_t bytes)
{
	if (bytes == 0)
		return false ;

	/// 용량 부족
	if ( bytes > mCapacity - mCurrentSize )
		return false ;
	
	// 바로 쓰기 가능한 경우
	if ( bytes <= mCapacity - mEndIndex )
	{
		memcpy(mData + mEndIndex, data, bytes) ;
		mEndIndex += bytes ;

		if ( mEndIndex == mCapacity )
			mEndIndex = 0 ;
	}
	// 쪼개서 써야 될 경우
	else
	{
		size_t size1 = mCapacity - mEndIndex ;
		memcpy(mData + mEndIndex, data, size1) ;
		
		size_t size2 = bytes - size1 ;
		memcpy(mData, data + size1, size2) ;
		mEndIndex = size2 ;
	}

	mCurrentSize += bytes ;

	return true ;
}

bool CircularBuffer::Read(char* data, size_t bytes)
{
	if (bytes == 0)
		return false ;

	if ( mCurrentSize < bytes )
		return false ;

	/// 바로 한번에 읽어 올 수 있는 경우
	if ( bytes <= mCapacity - mBeginIndex )
	{
		memcpy(data, mData + mBeginIndex, bytes) ;
		mBeginIndex += bytes ;

		if ( mBeginIndex == mCapacity )
			mBeginIndex = 0 ;
	}
	/// 읽어올 데이터가 쪼개져 있는 경우
	else
	{
		size_t size1 = mCapacity - mBeginIndex ;
		memcpy(data, mData + mBeginIndex, size1) ;

		size_t size2 = bytes - size1 ;
		memcpy(data + size1, mData, size2) ;
		mBeginIndex = size2 ;
	}

	mCurrentSize -= bytes ;

	return true ;
}

void CircularBuffer::Peek(char* data)
{
	/// 바로 한번에 읽어 올 수 있는 경우
	if ( mCurrentSize <= mCapacity - mBeginIndex )
	{
		memcpy(data, mData + mBeginIndex, mCurrentSize) ;
	}
	/// 읽어올 데이터가 쪼개져 있는 경우
	else
	{
		size_t size1 = mCapacity - mBeginIndex ;
		memcpy(data, mData + mBeginIndex, size1) ;

		size_t size2 = mCurrentSize - size1 ;
		memcpy(data + size1, mData, size2) ;
	}
}

bool CircularBuffer::Peek(char* data, size_t bytes)
{
	if (bytes == 0)
		return false ;

	if ( mCurrentSize < bytes )
		return false ;

	/// 바로 한번에 읽어 올 수 있는 경우
	if ( bytes <= mCapacity - mBeginIndex )
	{
		memcpy(data, mData + mBeginIndex, bytes) ;
	}
	/// 읽어올 데이터가 쪼개져 있는 경우
	else
	{
		size_t size1 = mCapacity - mBeginIndex ;
		memcpy(data, mData + mBeginIndex, size1) ;

		size_t size2 = bytes - size1 ;
		memcpy(data + size1, mData, size2) ;
	}

	return true ;
}


bool CircularBuffer::Consume(size_t bytes)
{
	if (bytes == 0)
		return false ;

	if ( mCurrentSize < bytes )
		return false ;

	/// 바로 한번에 제거할 수 있는 경우
	if ( bytes <= mCapacity - mBeginIndex )
	{
		mBeginIndex += bytes ;

		if ( mBeginIndex == mCapacity )
			mBeginIndex = 0 ;
	}
	/// 제거할 데이터가 쪼개져 있는 경우
	else
	{
		size_t size2 = bytes + mBeginIndex - mCapacity ;
		mBeginIndex = size2 ;
	}

	mCurrentSize -= bytes ;

	return true ;

}