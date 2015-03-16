#include "stdafx.h"
#include "Scheduler.h"


Scheduler::Scheduler()
{
	mCurrentTick = GetTickCount();
}


void Scheduler::PushTask(RefCountable* owner, Task&& task, uint32_t after)
{
	int64_t dueTimeTick = after + mCurrentTick;

	owner->IncRefCount(); ///< for scheduler
	mTaskQueue.push(JobElement(owner, std::move(task), dueTimeTick));
}


void Scheduler::DoTasks()
{
	/// tick update
	mCurrentTick = GetTickCount();

	while (!mTaskQueue.empty())
	{
		JobElement timerJobElem = mTaskQueue.top();

		if (mCurrentTick < timerJobElem.mExecutionTick)
			break;

		/// do task!
		timerJobElem.mTask();

		timerJobElem.mOwner->DecRefCount(); ///< for scheduler

		mTaskQueue.pop();
	}
}
