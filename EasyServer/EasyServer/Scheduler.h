#pragma once

#include <functional>
#include <queue>
#include "RefCountable.h"
#include "ThreadLocal.h"

typedef std::function<void()> Task;

struct JobElement
{
	JobElement(RefCountable* owner, const Task& task, int64_t execTick)
		: mOwner(owner), mTask(task), mExecutionTick(execTick)
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

	Scheduler();

	void PushTask(RefCountable* owner, const Task& task, uint32_t after);

	void DoTasks();

private:
	TaskPriorityQueue mTaskQueue;
	DWORD mCurrentTick;
};


template <class T, class F, class... Args>
void CallFuncAfter(uint32_t after, T instance, F memfunc, Args&&... args)
{
	static_assert(true == std::is_convertible<T, RefCountable*>::value, "only allowed when RefCountable*");

	auto bind = std::bind(memfunc, instance, std::forward<Args>(args)...);
	LScheduler->PushTask(static_cast<RefCountable*>(instance), bind, after);
}