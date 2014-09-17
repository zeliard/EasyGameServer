#pragma once

struct RefCountable
{
	RefCountable() : mRefCount(0)
	{}
	virtual ~RefCountable()
	{}

	void	IncRefCount()	{ ++mRefCount; }
	void	DecRefCount()	{ --mRefCount; }
	int		GetRefCount()	{ return mRefCount; }

	int mRefCount;
};
