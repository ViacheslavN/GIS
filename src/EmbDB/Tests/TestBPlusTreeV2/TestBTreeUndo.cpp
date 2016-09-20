#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPSetv2.h"
#include "../../EmbDB/Transactions.h"
#include "CommonLibrary/DebugTime.h"



typedef embDB::TBPSetV2 <int64,  embDB::comp<int64>, embDB::IDBTransaction> TBSet;




template<class TBtree, class Tran, class TKey>
void searchINBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, 
	int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bFind)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo();
	int64 nNotFound = 0;
	double searchTm  = 0;
	int64 n = 0;
	time.start();
	int64 nCount = nEndStart - nStart;
	for (__int64 i = nStart; i < nEndStart; ++i)
	{	
			TBtree::iterator it = tree.find(TKey(i));

			if(bFind)
			{
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
			}
			else
			{
				if(!it.isNull())
				{
					std::cout << "Error found " << i << std::endl;
				}
			}

			
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}

	searchTm = time.stop();
	std::cout << "Search end key start: " << nStart << " key end: " << nEndStart << " Not found: " << nNotFound << " Total time: " << searchTm << std::endl;

}


template<class TBtree, class Tran, class TKey>
void insertINBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, 
	CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bDeleteTran)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	int64 nCount = nEndStart - nStart;
	for (__int64 i = nStart; i < nEndStart; ++i)
	{

		if(!tree.insert(TKey(i)))
		{
			std::cout   << "Error Insert key:  " << i << std::endl;
		}
		n++;
		if(i%nStep == 0)
		{
			std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
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
	pTran->OutDebugInfo();


	std::cout << "Insert end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
}




template<class TBtree, class Tran, class TKey>
void removeFromBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, 
	int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bDeleteTran)
{
	std::cout << "Remove Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmRemove = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo();
	time.start();
	int64 n = 0;
	int64 nCount = nEndStart - nStart;
	for (int64 i = nStart; i < nEndStart; ++i)
	{	
			if(!tree.remove(TKey(i)))
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}
			n++;
			TBtree::iterator it = tree.find(TKey(i));
			if(!it.isNull())
			{
				std::cout << "Error remove,  found " << i << std::endl;
 
			}
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}

	
	tmRemove = time.stop();
	time.start();
	tree.commit();

	treeCom = time.stop();
	time.start();

	pTran->commit();
	tranCom = time.stop();
	pTran->OutDebugInfo();
	nTreeRootPage = tree.getPageBTreeInfo();

	std::cout << "Remove end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmRemove + treeCom + tranCom) <<
		" time remove: " << tmRemove << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
}

template<class TBtree,  class TTran, class TKey>
void testBPTreeSetUndoImpl (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, 
	int32 nCacheBPTreeSize, int32 nTranCache)
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
			storage.open(L"d:\\dbplus.data", false, false,  true, false);
			embDB::FilePagePtr pPage = storage.getNewPage(nPageSize);
			nStorageInfoPage = pPage->getAddr();
			storage.initStorage(pPage->getAddr());
			pPage.release();
			storage.saveStorageInfo();

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\tran1.data", &storage, 1, nTranCache);
			tran.begin();
			tran.setDeleteStorage(true);
			insertINBTreeSet <TBtree, TTran,  TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc, nTreeRootPage, true);
			std::cout << "File Size " << storage.getFileSize() << " StorageInfoPage " 
				<<  nStorageInfoPage << " nTreeRootPage " <<nTreeRootPage  << std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, L"d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage, true);
			storage.close();
		}


		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
	 

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\tran1.data", &storage, 1, nTranCache);
			tran.begin();
			tran.setDeleteStorage(false);
			insertINBTreeSet <TBtree, TTran,  TKey>(nCacheBPTreeSize, nCount, nCount + nCount/2, nStep, &tran, alloc, nTreeRootPage, true);
			std::cout << "File Size " << storage.getFileSize() << " StorageInfoPage " 
				<<  nStorageInfoPage << " nTreeRootPage " <<nTreeRootPage  << std::endl;
			
			storage.close();

			{

				embDB::CStorage storage1( alloc, nCacheStorageSize);
				storage1.setStoragePageInfo(nStorageInfoPage);
				storage1.open(L"d:\\dbplus.data", false, false,  false, false);
				storage1.loadStorageInfo();
				TTran Undotran(alloc,   embDB::rtUndefined, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage1, -1, nTranCache);
				Undotran.restore(true);
			}
			
			
		
		}


		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, L"d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage, true);
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, nCount, nCount + nCount/2, nStep, &tran1, alloc, nTreeRootPage, false);
			storage.close();
		}

		{
			nTreeRootPage = 6;
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\Remtran.data", &storage, 1, nTranCache);
			remtran.setDeleteStorage(false);
			remtran.begin();
			removeFromBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount/2, nStep, &remtran, alloc, nTreeRootPage, false);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, L"d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount/2, nStep, &tran1, alloc, nTreeRootPage, false);
			storage.close();
		}

		{

			embDB::CStorage storage1( alloc, nCacheStorageSize);
			storage1.setStoragePageInfo(nStorageInfoPage);
			storage1.open(L"d:\\dbplus.data", false, false,  false, false);
			storage1.loadStorageInfo();
			TTran Undotran(alloc,  embDB::rtUndefined, embDB::eTT_UNDEFINED, L"d:\\Remtran.data", &storage1, -1,nTranCache);
			Undotran.restore(true);
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, L"d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage, true);
			storage.close();
		}

	}

}

void testUndo()
{
	__int64 nCount = 1000000;
	size_t nPageSize = 8192;
	testBPTreeSetUndoImpl<TBSet, embDB::CTransaction, int64>(nCount, nPageSize, 1000, 10, 10000);
}