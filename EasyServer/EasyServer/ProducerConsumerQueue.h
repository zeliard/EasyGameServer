#pragma once

template<typename TElem, int QSize>
class ProducerConsumerQueue
{
public:
	ProducerConsumerQueue() : mOccupiedSize(0), mQueueOffset(0)
	{
		memset(mQueueArray, 0, sizeof(mQueueArray));
		InitializeConditionVariable(&mNotFull);
		InitializeConditionVariable(&mNotEmpty);
		InitializeSRWLock(&mLock);
	}

	~ProducerConsumerQueue() {}

	void Produce(const TElem& item)
	{
		AcquireSRWLockExclusive(&mLock);

		/// 큐에 넣을 공간 생길때까지 잔다.
		while (mOccupiedSize == QSize)
			SleepConditionVariableSRW(&mNotFull, &mLock, INFINITE);
		
		mQueueArray[(mQueueOffset + mOccupiedSize) % QSize] = item;
		++mOccupiedSize;

		ReleaseSRWLockExclusive(&mlock);

		WakeConditionVariable(&mNotEmpty);
	}

	
	void Consume(TElem& item)
	{
		AcquireSRWLockExclusive(&mLock);

		/// 큐에 아이템 들어올때까지 잔다
		while (mOccupiedSize == 0)
			SleepConditionVariableSRW(&mNotEmpty, &mLock, INFINITE);

		item = mQueueArray[mQueueOffset];
		--mOccupiedSize;

		if (++mQueueOffset == QSize)
			mQueueOffset = 0;

		ReleaseSRWLockExclusive(&mlock);

		WakeConditionVariable(&mNotFull);
	}

private:

	TElem mQueueArray[QSize];

	uint32_t mOccupiedSize;
	uint32_t mQueueOffset;

	CONDITION_VARIABLE mNotFull;
	CONDITION_VARIABLE mNotEmpty;

	SRWLOCK mLock;

};


