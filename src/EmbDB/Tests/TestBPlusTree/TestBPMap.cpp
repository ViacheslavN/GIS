#include "stdafx.h"
#include "TestBPTree.h"


#include "../../EmbDB/RBSet.h"
#include "../../EmbDB/RBMuitiSet.h"
#include "../../EmbDB/storage.h"
#include "../../EmbDB/BPTreeNode.h"
#include "../../EmbDB/BaseBPMap.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/MemoryStream.h"
#include "../../EmbDB/Key.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/BTVector.h"
#include "../../EmbDB/OIDField.h"
#include "../../EmbDB/BaseBPTreeRO.h"
#include "../../EmbDB/DirectTransactions.h"

#include "../../EmbDB/BPInnerNodeSimpleCompressor.h"
#include "../../EmbDB/BPLeafNodeSimpleCompressor.h"
#include "CommonLibrary/DebugTime.h"

#include "TestStringBPtree.h"
#include "TestBigInnerNode.h"


#include "../../EmbDB/BPInnerNodeSetROSimpleCompressor.h"
#include "../../EmbDB/BPLeafNodeMapROSimpleCompressor.h"
#include "../../EmbDB/BaseBPMapRO.h"



typedef embDB::BPInnerNodeSimpleCompressor<int64, int64, embDB::comp<int64> > TInnerCompressor;
typedef embDB::BPLeafNodeSimpleCompressor<int64, int64, embDB::comp<int64> > TLeafCompressor;

typedef embDB::TBPMap<int64,  int64, embDB::comp<int64>> TBTreePlus;




typedef embDB::BPInnerNodeSetROSimpleCompressor<int64, int64 > TInnerCompressorRO;
typedef embDB::BPLeafNodeMapROSimpleCompressor<int64, int64> TLeafCompressorRO;

typedef embDB::TBaseBPlusTreeMapRO<int64, int64, int64, embDB::comp<int64>, embDB::IDBTransaction> TBTreePlusRO;


 
 

typedef embDB::TBPlusTreeMap<int64, sStringStrustTest, int64, embDB::comp<int64>, 
	embDB::IDBTransaction, 
	embDB::TBPMapTraitsRBTreeBaseCustomCompress<int64, sStringStrustTest,
	int64, embDB::comp<int64>,
	embDB::IDBTransaction, 
	TInnerCompressor,
	BPStringLeafNodeCompressor >
> TBTreePlusString;

typedef embDB::TBaseBPlusTreeRO<int64, sStringStrustTest, int64, embDB::comp<int64>, embDB::IDBTransaction, TInnerCompressor, BPStringLeafNodeCompressor  > TBTreePlusROString;





typedef embDB::TBPMap<sStringInnerStrustTest, double, InnerComp, 
	BPStringInnerNodeCompressor,BPKeyStringLeafNodeCompressor
  > TBTreePlusInnerString;



typedef embDB::TBaseBPlusTreeRO<sStringInnerStrustTest, double, int64, InnerComp, embDB::IDBTransaction, BPStringInnerNodeCompressor, BPKeyStringLeafNodeCompressor  > TBTreePlusROInnerString;


template<class TBtree, class Tran, class TVal, class TKey>
void insertINBTree (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, TVal& val, int64& nTreeRootPage, int MultiCount = 1)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, MultiCount > 1);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	if(MultiCount < 1)
		MultiCount = 1;

	int64 nCount = abs(nEndStart - nStart) * MultiCount;
	for (int m = 0;  m < MultiCount; ++m)
	{
		if(nStart < nEndStart)
		{
			
			for (__int64 i = nStart; i < nEndStart; ++i)
			{
				tree.insert(TKey(i), val);
				n++;
				if(i%nStep == 0)
				{
					std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
				}
			}
		}
		else
		{
			for (__int64 i = nStart; i > nEndStart; --i)
			{
				tree.insert(TKey(i), val);
				n++;
				if(i%nStep == 0)
				{
					std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
				}
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
void searchINBTree (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, int MultiCount = 1)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, MultiCount > 1);
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
			TBtree::iterator it = tree.find(TKey(i), true);
			for (int d = 0; d < MultiCount; ++d)
			{
				if(it.isNull()  || it.key() != TKey(i))
				{
					std::cout << "Not found " << i << "for multi m:" << d << std::endl;
					nNotFound++;
					break;
					
				}
				else
					it.next();
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
			TBtree::iterator it = tree.find(TKey(i), true);
			for (int d = 0; d < MultiCount; ++d)
			{
				if(it.isNull()  || it.key() != TKey(i))
				{
					std::cout << "Not found " << i << "for multi m:" << d << std::endl;
					nNotFound++;
					break;
					
				}
				else
					it.next();
			}
			/*if(it.isNull() || it.key() != TKey(i))
			{
				std::cout << "Not found " << i << std::endl;
				
			}*/
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
void removeFromBTree (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, int MultiCount = 1)
{
	std::cout << "Remove Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmRemove = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, MultiCount > 1);
	tree.loadBTreeInfo();
	time.start();

	int64 n = 0;
	int64 nError = 0;
	if(MultiCount < 1)
		MultiCount = 1;
	int64 nCount = abs(nEndStart - nStart) *MultiCount;
	for (int m = 0; m < MultiCount; ++m)
	{
		int64 i = nStart;
		if(nStart < nEndStart)
		{
			
			for (; i < nEndStart; ++i)
			{	

				if(!tree.remove(TKey(i)))
				{
					std::cout << "Error remove :  " << i << '\n';
					nError++;
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
			for (; i > nEndStart; --i)
			{	

				if(!tree.remove(TKey(i)))
				{
					std::cout << "Error remove :  " << i << '\n';
					nError++;
				}
				n++;
				if(i%nStep == 0)
				{
					std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
				}
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

	std::cout << "Remove end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmRemove + treeCom + tranCom) << " Error :" << nError <<
		" time remove: " << tmRemove << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;
}

template<class TBtree, class Tran, class TKey>
void testOrderINBTree (int32 nCacheBPTreeSize, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, int  MultiCount = 1)
{
	std::cout << "Order Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double orderTm  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, MultiCount > 1);
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
				std::cout << "Order error key: " << nVal << " next key :" << it.key() << " step: " << nSize << std::endl;
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


template<class TBtree, class TBtreeRO, class TTran, class TValue, class TKey>
void testBPTreeMapImpl (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize ,TValue& val, int MultiCount = 1)
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

		int64 nStep =(MultiCount < 1 ? 1 : MultiCount) * nCount/100;
		if(nStep == 0)
			nStep = 1;
		{
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo.data", &storage, 1);
			tran.begin();
			insertINBTree<TBtree, TTran, TValue, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc,val, nTreeRootPage, MultiCount);
			std::cout << "FileSize :" << storage.getFileSize()/1000000 << "Mb "<<std::endl;
		}
	
		{
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo1.data", &storage, 1);
			tran1.begin();
			searchINBTree<TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage, MultiCount);

		}
		{
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo1.data", &storage, 1);
			tran1.begin();
			searchINBTree<TBtreeRO, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage, MultiCount);

		}

	

		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo5.data", &storage, 1);
			tran5.begin();
			testOrderINBTree<TBtreeRO, TTran, TKey>(nCacheBPTreeSize,  nStep,&tran5, alloc, nTreeRootPage, MultiCount);
			 
		}
		
	
		int64 mRemConst = nCount/2;
		{
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranRemUndo.data", &storage, 1);
			remtran.begin();
			removeFromBTree<TBtree, TTran, TKey>(nCacheBPTreeSize, nCount, mRemConst, nStep, &remtran, alloc, nTreeRootPage, MultiCount);
			std::cout << "FileSize :" << storage.getFileSize()/1000000 << "Mb "<<std::endl;
		}
			
		{
			TTran tran2(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo2.data", &storage, 1);
			tran2.begin();
			searchINBTree<TBtreeRO, TTran, TKey>(nCacheBPTreeSize, mRemConst, 0, nStep, &tran2, alloc, nTreeRootPage, MultiCount);
		
		}

		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo5.data", &storage, 1);
			tran5.begin();
			testOrderINBTree<TBtreeRO, TTran, TKey>( nCacheBPTreeSize, nStep, &tran5, alloc, nTreeRootPage, MultiCount);
			 
		}

		{
			TTran tran3(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo3.data", &storage, 1);
			tran3.begin();
			insertINBTree<TBtree, TTran, TValue, TKey>(nCacheBPTreeSize, mRemConst, nCount,  nStep, &tran3, alloc, val,nTreeRootPage, MultiCount);
			std::cout << "FileSize :" << storage.getFileSize()/1000000 << "Mb "<<std::endl;
		}
	
		{
			TTran tran4(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo4.data", &storage, 1);
			tran4.begin();
			searchINBTree<TBtreeRO, TTran, TKey>(nCacheBPTreeSize, 0, nCount,  nStep, &tran4, alloc, nTreeRootPage, MultiCount);
		}

		{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo5.data", &storage, 1);
			tran5.begin();
			testOrderINBTree<TBtreeRO, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, MultiCount);
			
		}

		
	}
	double tEnd = time.stop();
	std::cout <<"EndTest test total time: " << tEnd << std::endl;
};


void testBPTreeMap ()
{
	int64 nCount = 10;
	size_t nPageSize = 8192;
	int64 nVal = 1;
	double dd = 0.0;
	sStringStrustTest strTest;



	//testBPTreeMapImpl<TBTreePlusInnerString, TBTreePlusROInnerString, embDB::CDirectTransactions, double, sStringInnerStrustTest>(nCount, nPageSize, 50, 1000, dd);
	//testBPTreeMapImpl<TBTreePlusInnerString, TBTreePlusROInnerString, embDB::CTransactions, double, sStringInnerStrustTest>(nCount, nPageSize, dd);

	//testBPTreeMapImpl<TBTreePlusString, TBTreePlusROString, embDB::CDirectTransactions, sStringStrustTest, int64>(nCount, nPageSize, 50, 1000, strTest, 3);
	//testBPTreeMapImpl<TBTreePlusString, TBTreePlusROString, embDB::CTransactions, sStringStrustTest, int64>(nCount, nPageSize, strTest);
	
	nCount = 1000000;
	testBPTreeMapImpl<TBTreePlus, TBTreePlusRO, embDB::CTransaction, int64, int64>(nCount, nPageSize, 5000, 1000, nVal, 0);
	//testBPTreeMapImpl<TBTreePlus, TBTreePlusRO, embDB::CTransactions, int64, int64>(nCount, nPageSize, nVal);
	
	
	std::cout <<"end ";
	int i = 0;
	std::cin >> i;
}