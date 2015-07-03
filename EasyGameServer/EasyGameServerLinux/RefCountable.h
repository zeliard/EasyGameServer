#pragma once

struct RefCountable
{
	RefCountable() : mRefCount(0)
	{}
	virtual ~RefCountable()
	{}

	void IncRefCount()	
	{ 
		++mRefCount; 
	}
	
	void DecRefCount()
	{ 
		--mRefCount; 
		if (mRefCount == 0)
			OnRelease();
	}

	virtual void OnRelease() {}

	int mRefCount;
};
