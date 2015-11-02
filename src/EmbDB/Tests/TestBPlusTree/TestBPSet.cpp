#include "stdafx.h"
#include "../../EmbDB/RBSet.h"
#include "../../EmbDB/RBMuitiSet.h"
#include "../../EmbDB/storage.h"
#include "../../EmbDB/BPTreeNodeSet.h"
#include "../../EmbDB/BaseBPSet.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/MemoryStream.h"
#include "../../EmbDB/Key.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"

#include "../../EmbDB/BPInnerNodeSetSimpleCompressor.h"
#include "../../EmbDB/BPLeafNodeSetSimpleCompressor.h"
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/BaseBPTreeSetRO.h"

 

typedef embDB::TBPSet<int64, embDB::comp<int64>> TBTreeSet;
typedef embDB::TBaseBPlusTreeSetRO<int64, int64, embDB::comp<int64>, 
	embDB::IDBTransaction> TBTreeSetRO;






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

			tree.remove(TKey(i));
			n++;
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

template<class TBtree, class Tran, class TKey>
void testOrderINBTreeSet (int32 nCacheBPTreeSize, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Order Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double orderTm  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo();
	time.start();
	TBtree::iterator it = tree.begin();
	bool bFirst = true;
	TKey nVal = 0;
	int64 nSize = 0;
	int64 nOrderError = 0;
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
	orderTm = time.stop();
	std::cout << "Order end size: " << nSize << " error: " << nOrderError << " Total time: " << orderTm << std::endl;

}


template<class TBtree, class TBtreeRO, class TTran, class TKey>
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
			insertINBTreeSet <TBtree, TTran,  TKey>(nCacheBPTreeSize, nCount, 0, nStep, &tran, alloc, nTreeRootPage);
		}

		{
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo1.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
		}
		{
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo1.data", &storage, 1);
			tran1.begin();
			searchINBTreeSet <TBTreeSetRO, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
		}

		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo5.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtreeRO, TTran, TKey>(nCacheBPTreeSize,  nStep,&tran5, alloc, nTreeRootPage);
		}


		int64 mRemConst = nCount/2;
		{
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranRemUndo.data", &storage, 1);
			remtran.begin();
			removeFromBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, nCount, mRemConst, nStep, &remtran, alloc, nTreeRootPage);
		}

		{
			TTran tran2(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo2.data", &storage, 1);
			tran2.begin();
			searchINBTreeSet <TBtreeRO, TTran, TKey>(nCacheBPTreeSize, mRemConst, 0, nStep, &tran2, alloc, nTreeRootPage);
		}

		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo5.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtreeRO, TTran, TKey>( nCacheBPTreeSize, nStep, &tran5, alloc, nTreeRootPage);
		}

		{
			TTran tran3(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo3.data", &storage, 1);
			tran3.begin();
			insertINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, nCount,  nStep, &tran3, alloc, nTreeRootPage);
		}

		{
			TTran tran4(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo4.data", &storage, 1);
			tran4.begin();
			searchINBTreeSet <TBtreeRO, TTran, TKey>(nCacheBPTreeSize, 0, nCount,  nStep, &tran4, alloc, nTreeRootPage);
		}

		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo5.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtreeRO, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage);
		}


	}
	double tEnd = time.stop();
	std::cout <<"EndTest test total time: " << tEnd << std::endl;
};


void testBPTreeSet ()
{
	int64 nCount = 1000000000;
	size_t nPageSize = 1048576;
	int64 nVal = 1;
	double dd = 0.0;
	//sStringStrustTest strTest;



	//testBPTreeSetImpl<TBTreePlusInnerString, TBTreePlusROInnerString, embDB::CDirectTransactions, double, sStringInnerStrustTest>(nCount, nPageSize, 50, 1000, dd);
	//testBPTreeSetImpl<TBTreePlusInnerString, TBTreePlusROInnerString, embDB::CTransactions, double, sStringInnerStrustTest>(nCount, nPageSize, dd);

	//testBPTreeSetImpl<TBTreePlusString, TBTreePlusROString, embDB::CDirectTransactions, sStringStrustTest, int64>(nCount, nPageSize, strTest);
	//testBPTreeSetImpl<TBTreePlusString, TBTreePlusROString, embDB::CTransactions, sStringStrustTest, int64>(nCount, nPageSize, strTest);

	nCount = 1000000000;
	testBPTreeSetImpl<TBTreeSet, TBTreeSetRO, embDB::CDirectTransactions, int64>(nCount, nPageSize, 500, 20);
	//testBPTreeSetImpl<TBTreePlus, TBTreePlusRO, embDB::CTransactions, int64, int64>(nCount, nPageSize, nVal);


	std::cout <<"end ";
	int i = 0;
	std::cin >> i;
}
 