#pragma once

#include "Common.h"

class ThreadCache
{
public:
	void* Allocate(size_t size);
	void Deallocate(void *ptr, size_t size);

	//线程私有的freelist没有，向CentralCache要
	void* FetchFromCentralCache(size_t index, size_t size);
	//线程私有的太多需要还给CentralCache
	void ListTooLong(FreeList* freelist, size_t size);

private:
	FreeList _freelist[NLISTS];
};

_declspec (thread) static ThreadCache* tlist = nullptr;