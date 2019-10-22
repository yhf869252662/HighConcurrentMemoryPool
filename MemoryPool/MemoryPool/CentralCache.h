#pragma once

#include "Common.h"

class CentralCache
{
public:
	static CentralCache* GetInstance()
	{
		return &_inst;
	}
	//在spanlist中找，没有从PageCache拿一个span
	Span* GetOneSpan(SpanList& spanlist, size_t byte_size);
	//给ThreadCache一定内存
	size_t FetchRangeObj(void *& start, void *& end, size_t n, size_t byte_size);
	//还PageCache一个span
	void ReleaseListToSpans(void* start, size_t size);

private:
	SpanList _spanlist[NLISTS];
private:
	CentralCache()
	{}
	CentralCache(CentralCache&) = delete;
	static CentralCache _inst;
};
