#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPSetv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB//InnerNodeCompress.h"
#include "CommonLibrary/DebugTime.h"



typedef embDB::TBPSetV2 <int64,  embDB::comp<int64>, embDB::IDBTransaction, embDB::BPInnerNodeFieldCompressor> TBDoubleSet;
 


template<class TBtree, class Tran, class TKey>
void insertINBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
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
	bool bFind = false;
	if(nStart < nEndStart)
	{

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
	}
	else
	{
		int64 nCount = nStart - nEndStart;
		for (__int64 i = nStart; i > nEndStart; --i)
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
void insertINBTreeSetByIT (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo(); 
	tree.SetMinSplit(true);
	time.start();
	int64 n = 0;
	bool bFind = false;

		int nRet = 0;
		TBtree::iterator it;
		nRet = tree.insert(TKey(nStart), NULL, &it);
		if(nRet == -1)
		{
			std::cout   << "Error Insert key:  " << nStart << std::endl;
		}
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart + 1; i < nEndStart; ++i)
		{
			if(i == 261119)
			{
				int i = 0;
				i++;
			}
			nRet =tree.insert(TKey(i), &it, &it);
			if(it.key() != i) 
			{
				std::cout   << "Error Value iterator Insert key:  " << i << std::endl;
			}
			if(nRet == -1)
			{
				std::cout   << "Error Insert key:  " << i << std::endl;
			}
		/*	if(i == 261120)
			{
				TBtree::iterator Findit = tree.find(TKey(i));
				if(Findit.isNull())
				{
					std::cout << "Not found " << i << std::endl;
				}
				else if( i != Findit.key())
				{
					std::cout << "Key not EQ " << i << std::endl;
				}
			}*/

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
void searchINBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo();
	int64 nNotFound = 0;
	double searchTm  = 0;
	int64 n = 0;
	time.start();
 

	TBtree::iterator it = tree.find(TKey(nStart));
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart + 1; i < nEndStart; ++i)
		{	

			if(i == 261120)
			{
				int i = 0;
				i++;
			}
			it = tree.find(TKey(i), &it,  true);
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

template<class TBtree, class Tran, class TKey>
void testOrderINBTreeSet (int32 nCacheBPTreeSize, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage , bool bForward)
{
	std::cout << "Order Test"  << (bForward ? " Forward" : " Back ") << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double orderTm  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo();
	time.start();
		int64 nSize = 0;
		int64 nOrderError = 0;
		if(bForward)
		{
			TBtree::iterator it = tree.begin();
			bool bFirst = true;
			TKey nVal = 0;
			while(!it.isNull())
			{
				if(bFirst)
				{
					nVal = it.key();
					bFirst = false;
				}
				else
				{
					if((it.key() - nVal) > 1)
					{
						std::cout << "Order error key: " /*<< nVal << "next key :" << it.key() */<< std::endl;
						nOrderError++;
					}
					nVal = it.key();
				}
				it.next();
				nSize++;
				if(nSize%nStep == 0)
				{
					std::cout << nSize   << '\r';
				}
			}
		}
		else
		{
			TBtree::iterator it = tree.last();
			bool bFirst = true;
			TKey nVal = 0;
	
		
			while(!it.isNull())
			{
				if(bFirst)
				{
					nVal = it.key();
					bFirst = false;
				}
				else
				{
					if((nVal - it.key()) > 1)
					{
						std::cout << "Order error key: " /*<< nVal << "next key :" << it.key() */<< std::endl;
						nOrderError++;
					}
					nVal = it.key();

				}
				it.back();
				nSize++;
				if(nSize%nStep == 0)
				{
					std::cout << nSize   << '\r';
				}
			}
		}
	orderTm = time.stop();
	std::cout << "Order end size: " << nSize << " error: " << nOrderError << " Total time: " << orderTm << std::endl;

}



template<class TBtree, class Tran, class TKey>
void removeFromBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Remove Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmRemove = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo();
	time.start();
	int64 i = nStart;
	int64 n = 0;

	
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (; i < nEndStart; ++i)
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
			/*else if( i != it.key())
			{
				std::cout << "Key not EQ " << i << std::endl;
				nNotFound++;
			}*/

			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}

	}
	else
	{

		int64 nCount = nStart - nEndStart;
		 
		for (; i > nEndStart; --i)
		{	
			
			if(!tree.remove(TKey(i)))
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}
						
			TBtree::iterator it = tree.find(TKey(i));
			if(!it.isNull())
			{
				std::cout << "Error remove,  found " << i << std::endl;

			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
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
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;
}





template<class TBtree, class Tran, class TKey>
void removeFromBTreeSetByIT  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Remove Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmRemove = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo();
	time.start();
	int64 i = nStart;
	int64 n = 0;
	int64 nCount = nEndStart - nStart;
	bool bRet = false;
	TBtree::iterator it = tree.find(TKey(nStart));
    int64 nKey = it.key();
	for (; i < nEndStart; ++i)
	{	

			//it.m_nIndex =  it.m_pCurNode->count() - 1;
			if(i == 509)
			{
				int d = 0;
				d++;
			}
			bRet = tree.remove(TKey(i));


			if(!bRet)
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}
	
			n++;
						

			TBtree::iterator Findit = tree.find(TKey(i));
			if(!Findit.isNull())
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




template<class TBtree, class Tran, class TKey>
void removeFromBTreeSetByITLast  (int32 nCacheBPTreeSize,  int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Remove Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmRemove = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo();
	time.start();
	int64 i = 0;
 	int64 nCount = tree.m_BTreeInfo.m_nLeafNodeCounts;
	int bRet = 0;
	int64 n = 0;
	int64 nKey = 0;
	TBtree::iterator it = tree.find(TKey(0));
	it.m_nIndex =  it.m_pCurNode->count() - 1;
	nKey = it.key();
	for (; i < tree.m_BTreeInfo.m_nLeafNodeCounts; ++i)
	{	
		if(i == 869)
		{
			TBtree::iterator FindKey = tree.find(it.key());
			TBtree::iterator Findit = tree.find(it.key() + 1);
			int dd = 0;
			dd++;
		}
		//it.m_nIndex =  it.m_pCurNode->count() - 1;
		it = tree.remove(it, bRet);
		if(bRet == -1)
		{
			std::cout << "Error remove,  not found " << i << std::endl;
			break;
		}
		if(it.isNull())
			break;

		

		TBtree::iterator Findit = tree.find(TKey(nKey));
		if(!Findit.isNull())
		{
			std::cout << "Error remove,  found " << i << std::endl;

		} 
		if(i%nStep == 0)
		{
			std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
		}

		it.m_nIndex =  it.m_pCurNode->count() - 1;
		n++;
		nKey = it.key();
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

	std::cout << "Remove end key start: " << 0 << " key end: " << n << " Total time: " << (tmRemove + treeCom + tranCom) <<
		" time remove: " << tmRemove << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;
}

template<class TBtree,  class TTran, class TKey>
void testBPTreeSetImpl (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize, int32 nTranCache)
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


			{
				TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\db\\createtran.data", &storage, 1);
				tran.begin();
				embDB::FilePagePtr pPage = tran.getNewPage(256);

				nTreeRootPage = pPage->getAddr();
				TBtree tree(-1, &tran, alloc, nCacheBPTreeSize, 8192);
				tree.init(nTreeRootPage); 

				tran.commit();
			}

	
	 


			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1, nTranCache);
			tran.begin();
			insertINBTreeSetByIT <TBtree, TTran,  TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() << " StorageInfoPage " 
				<<  nStorageInfoPage << " nTreeRootPage " <<nTreeRootPage  << std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran9.data", &storage, 1, nTranCache);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, true);
			storage.close();
		}

	//	nTreeRootPage = 6;
	
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
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
		}*/
	
	
		/*{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran4.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, false);
		}*/
	//	int64 mRemConst =246762;
	//	nTreeRootPage = 6;
		int64 mRemConst =nCount/2;
		//mRemConst =24;
	
		/*{
			nTreeRootPage = 6;
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran5.data", &storage, 1, nTranCache);
			remtran.begin();
			removeFromBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, 0, nStep, &remtran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}*/

		{
			//nTreeRootPage = 6;
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\tran5.data", &storage, 1, nTranCache);
			remtran.begin();
			removeFromBTreeSetByIT <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, mRemConst, nStep, &remtran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}


	/*	{
			nTreeRootPage = 6;
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran5.data", &storage, 1, nTranCache);
			remtran.begin();
			removeFromBTreeSetByITLast <TBtree, TTran, TKey>(nCacheBPTreeSize, nStep, &remtran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}
			return;*/
		
 		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran6.data", &storage, 1, nTranCache);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, nCount, nStep, &tran1, alloc, nTreeRootPage);
			storage.close();
		}
	
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran7.data", &storage, 1, nTranCache);
			tran.begin();
			insertINBTreeSet <TBtree, TTran,  TKey>(nCacheBPTreeSize, mRemConst, 0, nStep, &tran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran8.data", &storage, 1, nTranCache);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
			storage.close();
		}
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran9.data", &storage, 1, nTranCache);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, true);
			storage.close();
		}
	}
 
}





void TestBRteeSet()
{
	//__int64 nCount = 1531;
	//3130
	//__int64 nCount = 520201;
	__int64 nCount = 1000000;
	//nCount = 200;
	//	size_t nPageSize = 100;
	size_t nPageSize = 8192;
	testBPTreeSetImpl<TBDoubleSet, embDB::CDirectTransaction, int64>(nCount, nPageSize, 10000, 10, 10000);
}