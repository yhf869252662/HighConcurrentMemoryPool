#pragma once

#include "Common.h"

class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_inst;
	}

	Span* AllocBigPageObj(size_t size);
	void FreeBigPageObj(void* ptr, Span* span);

	Span* NewSpan(size_t n);
	Span* _NewSpan(size_t n);

	//��ȡ�Ӷ���span��ӳ��
	Span* MapObjectToSpan(void* obj);
	//�ͷſ���span�ص�Pagecache���ϲ����ڵ�span
	void ReleaseSpanToPageCache(Span* span);	
private:
	//std::map<PageID, Span*> _idspanmap;
	std::unordered_map<PageID, Span*> _idspanmap;	
	SpanList _spanlist[NPAGES];

	std::mutex _mutex;

private:
	static PageCache _inst;
	PageCache()
	{}
	PageCache(const PageCache&) = delete;
};