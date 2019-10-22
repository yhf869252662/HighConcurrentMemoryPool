#include "PageCache.h"
#include "Common.h"

PageCache PageCache::_inst;

Span* PageCache::AllocBigPageObj(size_t size)
{
	assert(size > MAX_SIZE);

	size = SizeClass::RoundUp(size);
	size_t npage = size >> PAGE_SHIFT;
	if (npage < NPAGES)
	{
		Span* span = NewSpan(npage);
		span->_objsize = size;
		return span;
	}
	else
	{
		void* ptr = VirtualAlloc(0, npage << PAGE_SHIFT,
			MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (ptr == nullptr)
			throw std::bad_alloc();

		Span* span = new Span;
		span->_npage = npage;
		span->_pageid = (PageID)ptr >> PAGE_SHIFT;
		span->_objsize = npage << PAGE_SHIFT;

		_idspanmap[span->_pageid] = span;

		return span;
	}
}

void PageCache::FreeBigPageObj(void* ptr, Span* span)
{
	size_t npage = span->_objsize >> PAGE_SHIFT;
	if (npage < NPAGES)
	{
		span->_objsize = 0;
		ReleaseSpanToPageCache(span);
	}
	else
	{
		_idspanmap.erase(npage);
		delete span;
		VirtualFree(ptr, 0, MEM_RELEASE);
	}

}

Span* PageCache::NewSpan(size_t n)
{
	std::unique_lock<std::mutex> lock(_mutex);

	return _NewSpan(n);
}

Span* PageCache::_NewSpan(size_t n)
{
	assert(n < NPAGES);

	if (!_spanlist[n].Empty())
		return _spanlist[n].PopFront();

	for (size_t i = n + 1; i < NPAGES; ++i)
	{
		if (!_spanlist[i].Empty())
		{
			Span* span = _spanlist[i].PopFront();
			Span* split = new Span;

			split->_pageid = span->_pageid;
			split->_npage = n;
			span->_pageid = span->_pageid + n;
			span->_npage -= n;

			for (size_t i = 0; i < n; ++i)
			{
				_idspanmap[split->_pageid + i] = split;
			}
			_spanlist[span->_npage].PushFront(span);
			return split;
		}
	}

	Span* span = new Span;

	// ������˵��SpanList��û�к��ʵ�span,ֻ����ϵͳ����128ҳ���ڴ�
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, (NPAGES - 1)*(1 << PAGE_SHIFT), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	//  brk
#endif

	span->_pageid = (PageID)ptr >> PAGE_SHIFT;//��������뵽�ĵ�ַ��ӦpageID
	span->_npage = NPAGES - 1;
	for (size_t i = 0; i < span->_npage; ++i)
	{
		_idspanmap[span->_pageid + i] = span;
	}
	_spanlist[span->_npage].PushFront(span);
	return _NewSpan(n);
}

//��ȡ�Ӷ���span��ӳ��
Span* PageCache::MapObjectToSpan(void* obj)
{
	PageID pageid = (PageID)obj >> PAGE_SHIFT;

	auto it = _idspanmap.find(pageid);

	if (it != _idspanmap.end())
	{
		return it->second;
	}
	else
	{
		assert(false);
		return nullptr;
	}
	
}

//�ͷſ���span�ص�Pagecache���ϲ����ڵ�span
void PageCache::ReleaseSpanToPageCache(Span* cur)
{
	std::unique_lock<std::mutex> lock(_mutex);

	// ���ͷŵ��ڴ��Ǵ���128ҳ,ֱ�ӽ��ڴ�黹������ϵͳ,���ܺϲ�
	if (cur->_npage >= NPAGES)
	{
		void* ptr = (void*)(cur->_pageid << PAGE_SHIFT);
		// �黹֮ǰɾ����ҳ��span��ӳ��
		_idspanmap.erase(cur->_pageid);
		VirtualFree(ptr, 0, MEM_RELEASE);
		delete cur;
		return;
	}

	////��ǰ�ϲ�
	while (1)
	{
		PageID curid = cur->_pageid;
		PageID previd = curid - 1;
		auto it = _idspanmap.find(previd);
		if (it == _idspanmap.end())
			break;

		Span *prev = it->second;

		if (prev->_usecount != 0)
			break;
		//����128ҳ�򲻺ϲ�
		if (cur->_npage + prev->_npage > NPAGES - 1)
			break;

		_spanlist[prev->_npage].Erase(prev);
		prev->_npage += cur->_npage;
		//����id->span��ӳ���ϵ
		for (PageID i = 0; i < cur->_npage; ++i)
		{
			_idspanmap[cur->_pageid + i] = prev;
		}
		delete cur;

		cur = prev;
	}


	////���ϲ�
	while (1)
	{
		PageID curid = cur->_pageid;
		PageID nextid = curid + cur->_npage;
		auto it = _idspanmap.find(nextid);
		if (it == _idspanmap.end())
			break;

		Span *next = it->second;

		if (next->_usecount != 0)
			break;

		//����128ҳ�򲻺ϲ�
		if (cur->_npage + next->_npage >= NPAGES - 1)
			break;

		_spanlist[next->_npage].Erase(next);
		cur->_npage += next->_npage;

		//����id->Span��ӳ���ϵ
		for (PageID i = 0; i < next->_npage; ++i)
		{
			_idspanmap[next->_pageid + i] = cur;
		}

		delete next;
	}

	_spanlist[cur->_npage].PushFront(cur);
}

