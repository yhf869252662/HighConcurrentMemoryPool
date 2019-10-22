//#include "Common.h"
//#include "ConcurrentAlloc.h"
//#include "PageCache.h"
//
//void testSizeClass()
//{
//	cout << SizeClass::Index(15) << endl;
//	cout << SizeClass::Index(8) << endl;
//	cout << SizeClass::Index(21) << endl;
//	cout << SizeClass::RoundeUp(15) << endl;
//	cout << SizeClass::RoundeUp(8) << endl;
//	cout << SizeClass::RoundeUp(21) << endl;
//}
//
//void Alloc(size_t n)
//{
//	vector<void*> v;
//
//	for (size_t i = 0; i < n; ++i)
//	{
//		v.push_back(ConcurrentAlloc(10));
//	}
//
//	for (size_t i = 0; i < n; ++i)
//	{
//		ConcurrentFree(v[i], 10);
//		cout << v[i] << endl;
//	}
//	v.clear();
//
//	for (size_t i = 0; i < n; ++i)
//	{
//		v.push_back(ConcurrentAlloc(10));
//	}
//
//	for (size_t i = 0; i < n; ++i)
//	{
//		ConcurrentFree(v[i], 10);
//		cout << v[i] << endl;
//	}
//	v.clear();
//}
//
//void testThreadCache()
//{
//	std::thread t1(Alloc, 10);
//	std::thread t2(Alloc, 10);
//	std::thread t3(Alloc, 10);
//	std::thread t4(Alloc, 10);
//	std::thread t5(Alloc, 10);
//
//	t1.join();
//	t2.join();
//	t3.join();
//	t4.join();
//	t5.join();
//}
//
//void testCentralCache()
//{
//	std::vector<void*> v;
//	for (size_t i = 0; i < 8; ++i)
//	{
//		v.push_back(ConcurrentAlloc(10));
//	}
//
//	for (size_t i = 0; i < 8; ++i)
//	{
//		ConcurrentFree(v[i], 10);
//		cout << v[i] << endl;
//	}
//}
//
//void TestPageCache()
//{
//	PageCache::GetInstance()->NewSpan(2);
//}
//
//int main()
//{
//	//testSizeClass();
//	testThreadCache();
//	//testCentralCache();
//	//TestPageCache();
//
//	/*void* ptr = VirtualAlloc(0, (2)*(1 << PAGE_SHIFT),
//		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
//	PageID id = (PageID)ptr >> PAGE_SHIFT;
//	cout << id << endl;
//
//	char* obj = (char*)ptr + 100;
//	PageID objid = (PageID)obj >> PAGE_SHIFT;
//	void* pageptr = (void*)(id << PAGE_SHIFT);
//	cout << pageptr << endl;
//	cout << ptr << endl;*/
//	
//	return 0;
//}