#pragma once

#include <functional>
#include <queue>
#include <chrono>
#include <stdint.h>
#include "RefCountable.h"
#include "ThreadLocal.h"

typedef std::function<void()> Task;

struct JobElement
{
	JobElement(RefCountable* owner, Task&& task, int64_t execTick)
		: mOwner(owner), mTask(std::move(task)), mExecutionTick(execTick)
	{}

	RefCountable* mOwner;
	Task		mTask;
	int64_t		mExecutionTick;
};

struct JobComparator
{
	bool operator()(const JobElement& lhs, const JobElement& rhs)
	{
		return lhs.mExecutionTick > rhs.mExecutionTick;
	}
};


typedef std::priority_queue<JobElement, std::vector<JobElement>, JobComparator> TaskPriorityQueue;

class Scheduler
{
public:
	using Clock = std::chrono::high_resolution_clock;
	Scheduler();

	void PushTask(RefCountable* owner, Task&& task, uint32_t after);

	void DoTasks();

private:
	int64_t GetCurrentTick()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - mBeginTickPoint).count();
	}

	TaskPriorityQueue mTaskQueue;
	Clock::time_point mBeginTickPoint;
	int64_t mCurrentTick;
};


template <class T, class F, class... Args>
void CallFuncAfter(uint32_t after, T instance, F memfunc, Args&&... args)
{
	static_assert(true == std::is_convertible<T, RefCountable*>::value, "only allowed when RefCountable*");
	
	LScheduler->PushTask(static_cast<RefCountable*>(instance), std::bind(memfunc, instance, std::forward<Args>(args)...), after);
}