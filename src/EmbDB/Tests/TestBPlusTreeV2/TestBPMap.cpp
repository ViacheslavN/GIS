#include "stdafx.h"
#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPMapv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "CommonLibrary/DebugTime.h"


typedef embDB::TBPMapV2 <int64,  int64, embDB::comp<int64>, embDB::IDBTransactions> TBInt64Map;


template<class TBtree, class Tran, class TKey, class TValue>
void insertINBTreeMap  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			if(i == 130560)
			{
				int dd = 0;
				dd++;
			}
			if(!tree.insert(TKey(i), TValue(i)))
			{
				std::cout   << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}
	else
	{
		int64 nCount = nStart - nEndStart;
		for (__int64 i = nStart; i > nEndStart; --i)
		{
			if(!tree.insert(TKey(i), TValue(i)))
			{
				std::cout   << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}
	tmInsert = time.stop();
	time.start();
	tree.commit();

	treeCom = time.stop();
	time.start();
	pTran->commit();
	tranCom = time.stop();
	nTreeRootPage = tree.getPageBTreeInfo();


	std::cout << "Insert end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;

}




template<class TBtree, class Tran, class TKey, class TValue>
void searchINBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo();
	int64 nNotFound = 0;
	double searchTm  = 0;
	int64 n = 0;
	time.start();

	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{	
			TBtree::iterator it = tree.find(TKey(i));
			if(it.isNull())
			{
				std::cout << "Not found " << i << std::endl;
				nNotFound++;
			}
			else if( i != it.key())
			{
				std::cout << "Key not EQ " << i << std::endl;
				nNotFound++;
			}
			else if(it.key() != it.value())
			{
				std::cout << "Key not EQ value " << i << std::endl;
				nNotFound++;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}
	else
	{
		int64 nCount = nStart - nEndStart;
		for (__int64 i = nStart; i > nEndStart; --i)
		{	
			TBtree::iterator it = tree.find(TKey(i));
			if(it.isNull())
			{
				std::cout << "Not found " << i << std::endl;
				nNotFound++;
			}
			else if( i != it.key())
			{
				std::cout << "Key not EQ " << i << std::endl;
				nNotFound++;
			}
			else if(it.key() != it.value())
			{
				std::cout << "Key not EQ value " << i << std::endl;
				nNotFound++;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}

	searchTm = time.stop();
	std::cout << "Search end key start: " << nStart << " key end: " << nEndStart << " Not found: " << nNotFound << " Total time: " << searchTm << std::endl;

}

template<class TBtree,  class TTran, class TKey, class TValue>
void testBPTreeMapImpl (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize)
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout <<"Begin test Count: " << nCount << " PageSize :" << nPageSize << std::endl;
	{
		
		int64 nTreeRootPage = -1;
		int64 nStorageInfoPage = 0;
		int64 nStep = nCount/100;
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  true, false, nPageSize);
			embDB::CFilePage* pPage = storage.getNewPage();
			nStorageInfoPage = pPage->getAddr();
			storage.initStorage(pPage->getAddr());

			storage.saveStorageInfo();

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1);
			tran.begin();
			insertINBTreeMap <TBtree, TTran,  TKey, TValue>(nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey, TValue>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
			storage.close();
		}
		/*{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran3.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, true);
			storage.close();
		}
		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran4.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, false);
		}
	//	int64 mRemConst =246762;
		int64 mRemConst =nCount/2;
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran5.data", &storage, 1);
			remtran.begin();
			removeFromBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, 0, nStep, &remtran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}
	
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran6.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, nCount, nStep, &tran1, alloc, nTreeRootPage);
			storage.close();
		}
	
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran7.data", &storage, 1);
			tran.begin();
			insertINBTreeSet <TBtree, TTran,  TKey>(nCacheBPTreeSize, mRemConst, 0, nStep, &tran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran8.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
			storage.close();
		}
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran9.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, true);
			storage.close();
		}*/
	}
 
}





void TestBRteeMap()
{
	//__int64 nCount = 1531;

	__int64 nCount = 1000000;
		size_t nPageSize = 8192;

	testBPTreeMapImpl<TBInt64Map,  embDB::CDirectTransactions, int64, int64>(nCount, nPageSize, 5000, 1000);
}