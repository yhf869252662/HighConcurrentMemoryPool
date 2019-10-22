#pragma once

#include "ThreadCache.h"
#include "PageCache.h"

static inline void* ConcurrentAlloc(size_t size)
{
	if (size > MAX_SIZE)
	{

		Span* span = PageCache::GetInstance()->AllocBigPageObj(size);
		void *ptr = (void*)(span->_pageid << PAGE_SHIFT);
		return ptr;
		//return malloc(size);
	}
	else
	{
		if (tlist == nullptr)
		{
			tlist = new ThreadCache;
		}
		return tlist->Allocate(size);
	}
	
}

static inline void ConcurrentFree(void *ptr)
{
	Span* span = PageCache::GetInstance()->MapObjectToSpan(ptr);
	size_t size = span->_objsize;
	if (size > MAX_SIZE)
	{
		//free(ptr);
		PageCache::GetInstance()->FreeBigPageObj(ptr, span);
	}
	else
	{
		tlist->Deallocate(ptr, size);
	}
}
