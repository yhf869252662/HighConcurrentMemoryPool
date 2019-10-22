#include "ThreadCache.h"
#include "CentralCache.h"

void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	FreeList* freelist = &_freelist[index];
	void *start = nullptr;
	void *end = nullptr;
	size_t num = SizeClass::NumMoveSize(size);
	size_t maxsize = freelist->MaxSize();
	size_t n = min(num, maxsize);
	size_t batchsize = CentralCache::GetInstance()->FetchRangeObj(start, end, n, size);

	//拿到超过一个的其余的挂起来
	if (batchsize > 1)
	{
		freelist->PushRange(NextObj(start), end, batchsize - 1);
	}

	//动态修改maxsize
	if (batchsize >= maxsize)
		freelist->SetMaxSize(maxsize + 1);

	return start;
}

void* ThreadCache::Allocate(size_t size)
{
	size_t index = SizeClass::Index(size);
	FreeList *freelist = &_freelist[index];
	if (!freelist->Empty())
	{
		return freelist->Pop();
	}
	else
	{
		return FetchFromCentralCache(index, SizeClass::RoundUp(size));
	}
}

void ThreadCache::Deallocate(void *ptr, size_t size)
{
	size_t index = SizeClass::Index(size);
	FreeList *freelist = &_freelist[index];
	freelist->Push(ptr);

	if (freelist->Size() >= freelist->MaxSize())
	{
		ListTooLong(freelist, size);
	}
}

void ThreadCache::ListTooLong(FreeList* freelist, size_t size)
{
	void *start = freelist->PopRange();
	CentralCache::GetInstance()->ReleaseListToSpans(start, size);
}



