#include "stdafx.h"
#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPSetv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "CommonLibrary/DebugTime.h"


typedef embDB::TBPlusTreeSetV2 <int64, int64, embDB::comp<int64>, embDB::IDBTransactions> TBDoubleSet;
 


template<class TBtree, class Tran, class TKey>
void insertINBTreeSet  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
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
			tree.insert(TKey(i));
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
			tree.insert(TKey(i));
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



template<class TBtree, class Tran, class TKey>
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
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
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
					if(999999 == nVal)
					{
						int dd = 0;
						dd++;
					}
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
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo();
	time.start();
	int64 i = nStart;
	int64 n = 0;
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (; i < nEndStart; ++i)
		{	

			if(i == 1530)
			{
				int dd = 0;
				dd++;
			}
			tree.remove(TKey(i));
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
		TBtree::iterator it = tree.find(TKey(54));
		int64 nCount = nStart - nEndStart;
		for (; i > nEndStart; --i)
		{	

			tree.remove(TKey(i));
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

	nTreeRootPage = tree.getPageBTreeInfo();

	std::cout << "Remove end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmRemove + treeCom + tranCom) <<
		" time remove: " << tmRemove << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;
}


template<class TBtree,  class TTran, class TKey>
void testBPTreeSetImpl (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize)
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout <<"Begin test Count: " << nCount << " PageSize :" << nPageSize << std::endl;
	{
		embDB::CStorage storage( alloc, nCacheStorageSize);
		storage.open("d:\\dbplus.data", false, false,  true, false, nPageSize);
		embDB::CFilePage* pPage = storage.getNewPage();
		storage.setStoragePageInfo(pPage->getAddr());
		storage.saveStorageInfo();

		int64 nTreeRootPage = -1;

		int64 nStep = nCount/100;
		{
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndoset.data", &storage, 1);
			tran.begin();
			insertINBTreeSet <TBtree, TTran,  TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc, nTreeRootPage);
		}
	/*	{
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo1.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
		}

		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranOrderF.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, true);
		}
		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranOrderBack.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, false);
		}*/

		int64 mRemConst = 500000;
		{
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranRemUndo.data", &storage, 1);
			remtran.begin();
			removeFromBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, mRemConst, nStep, &remtran, alloc, nTreeRootPage);
		}

		{
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo2.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, nCount, nStep, &tran1, alloc, nTreeRootPage);
		}

	}
 
}





void TestBRteeSet()
{
	__int64 nCount = 1000000;
		size_t nPageSize = 8192;
	testBPTreeSetImpl<TBDoubleSet,  embDB::CDirectTransactions, int64>(nCount, nPageSize, 5000, 1000);
}