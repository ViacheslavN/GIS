#include "stdafx.h"
#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
/*#include "../../EmbDB/BTreePlusv3/BaseBPSetv3.h"
#include "../../EmbDB/BTreePlusv3/BPIteratorSetV3.h"*/


#include "../../EmbDB/BTreePlusv3/BPSetv3.h"

#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/ConsolLog.h"
#include "CommonLibrary/DebugTime.h"

/*
template <	class _TKey, class _TComp, class _Transaction,
class _TInnerCompess = BPInnerNodeSimpleCompressorV3<_TKey>,
class _TLeafCompess = BPLeafNodeSetSimpleCompressorV3<_TKey>,
class _TInnerNode = BPTreeInnerNodeSetv3<_TKey, _Transaction, _TInnerCompess>,
class _TLeafNode = BPTreeLeafNodeSetv3<_TKey,  _Transaction, _TLeafCompess>,
class _TBTreeNode = BPTreeNodeSetv3<_TKey, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
>
class TBPlusTreeSetV3
*/

/*
template <class _TKey, class _TComp, class _Transaction, class _TInnerCompess, class _TLeafCompess,
class _TInnerNode, class _TLeafNode, class _TBTreeNode>
class TBPSetIteratorV3
*/
typedef embDB::comp<int64> TComparator;
typedef embDB::BPInnerNodeSimpleCompressorV3<int64> TInnerCompess;
typedef embDB::BPTreeInnerNodeSetv3<int64, embDB::IDBTransaction, TInnerCompess> TInnerNode;
typedef embDB::BPLeafNodeSetSimpleCompressorV3<int64> TLeafCompess;
typedef embDB::BPTreeLeafNodeSetv3<int64, embDB::IDBTransaction, TLeafCompess> TLeafNode;
typedef embDB::BPTreeNodeSetv3<int64, embDB::IDBTransaction, TInnerCompess, TLeafCompess, TInnerNode, TLeafNode> TBTreeNode;

//typedef embDB::TBPSetIteratorV3<int64, TComparator, embDB::IDBTransaction, TInnerCompess, TLeafCompess, TInnerNode, TLeafNode, TBTreeNode> TIterator;

typedef embDB::TBPSetV3<int64, TComparator, embDB::IDBTransaction, TInnerCompess, TLeafCompess, TInnerNode, TLeafNode, TBTreeNode> TBPSet64;





template<class TBtree, class Tran, class TKey, class TValue>
void searchINBTreeSet(uint32 nPageSize, int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bCheckCRC)
{
   std::cout << "Search Test" << std::endl;
	 
   TComparator m_comp;

	CommonLib::TimeUtils::CDebugTime time;


	

	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, nPageSize, false, bCheckCRC);
	tree.loadBTreeInfo();
	int64 nNotFound = 0;
	double searchTm = 0;
	int64 n = 0;
	time.start();

	if (nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			auto it = tree.find(TKey(i));
			if (it.isNull())
			{
				std::cout << "Not found " << i << std::endl;
				nNotFound++;
			}
			else if (i != it.key())
			{
				std::cout << "Key not EQ " << i << std::endl;
				nNotFound++;
			}
			n++;
			if (i%nStep == 0)
			{
				std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
			}
		}
	}
	else
	{
		int64 nCount = nStart - nEndStart;
		for (__int64 i = nStart; i > nEndStart; --i)
		{
			auto it = tree.find(TKey(i));
			if (it.isNull())
			{
				std::cout << "Not found " << i << std::endl;
				nNotFound++;
			}
			else if (i != it.key())
			{
				std::cout << "Key not EQ " << i << std::endl;
				nNotFound++;
			}
			n++;
			if (i%nStep == 0)
			{
				std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
			}
		}
	}

	searchTm = time.stop();
	std::cout << "Search end key start: " << nStart << " key end: " << nEndStart << " Not found: " << nNotFound << " Total time: " << searchTm << std::endl;
}



template<class TBtree, class Tran, class TKey, class TValue>
void insertINBTreeSet(uint32 nPageSize, int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bCheckCRC)
{
	std::cout << "Insert Test" << std::endl;
 
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, nPageSize, false, bCheckCRC);
	tree.loadBTreeInfo();
	if (nStart < nEndStart)
		tree.SetMinSplit(true);
 
	time.start();
	int64 n = 0;
	if (nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			if (i == 1675526)
			{
				int dd = 0;
				dd++;
			}
			if (!tree.insert(TKey(i)))
			{
				std::cout << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if (i%nStep == 0)
			{
				std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
			}
		}
	}
	else
	{
		int64 nCount = nStart - nEndStart;
		for (__int64 i = nStart; i > nEndStart; --i)
		{
			if (!tree.insert(TKey(i)))
			{
				std::cout << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if (i%nStep == 0)
			{
				std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
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
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom << std::endl;
 

}





template<class TBtree, class Tran, class TKey>
void removeFromBTreeSet(uint32 nPageSize, int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage, bool bCheckCRC)
{
	std::cout << "Remove Test" << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmRemove = 0;
	double treeCom = 0;
	double tranCom = 0;
 
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, nPageSize, false, bCheckCRC);
	tree.loadBTreeInfo();

	time.start();
	int64 i = nStart;
	int64 n = 0;


	if (nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (; i < nEndStart; ++i)
		{


			if (!tree.remove(TKey(i)))
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}
			n++;
			auto it = tree.find(TKey(i));
			if (!it.isNull())
			{
				std::cout << "Error remove,  found " << i << std::endl;

			}


			if (i%nStep == 0)
			{
				std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
			}
		}

	}
	else
	{

		int64 nCount = nStart - nEndStart;

		for (; i > nEndStart; --i)
		{

			if (!tree.remove(TKey(i)))
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}

			auto it = tree.find(TKey(i));
			if (!it.isNull())
			{
				std::cout << "Error remove,  found " << i << std::endl;

			}
			n++;
			if (i%nStep == 0)
			{
				std::cout << n << "  " << (n * 100) / nCount << " %" << '\r';
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
 

	std::cout << "Remove end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmRemove + treeCom + tranCom) <<
		" time remove: " << tmRemove << " time tree commit: " << treeCom << " Tran commit: " << tranCom << std::endl;

}



template<class TBtree, class TTran, class TKey, class TValue>
void testBPTreeSetImpl(int64 nCount, uint32 nTranCache, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize, bool bCheckCRC)
{

	typedef typename TBtree::TInnerCompressorParams TInnerCompressorParams;
	typedef typename TBtree::TLeafCompressorParams TLeafCompressorParams;

	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout << "Begin test Count: " << nCount << " PageSize :" << nPageSize << std::endl;
	int64 nTreeRootPage = -1;
	int64 nStorageInfoPage = 0;
	int64 nStep = nCount / 100;
	{
	

		{
			embDB::CStorage storage(alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false, true, false);
			embDB::FilePagePtr pPage = storage.getNewPage(nPageSize);
			nStorageInfoPage = pPage->getAddr();
			storage.initStorage(pPage->getAddr());
			//pPage.release();
			storage.saveStorageInfo();

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\db\\createtran.data", &storage, 1, nTranCache);
			 
			tran.begin();
			embDB::FilePagePtr pRootPage = tran.getNewPage(256);

			nTreeRootPage = pRootPage->getAddr();
			TBtree tree(-1, &tran, alloc, nCacheBPTreeSize, nPageSize, false, bCheckCRC);
			TInnerCompressorParams inerComp;
			TLeafCompressorParams leafComp;
			tree.init(nTreeRootPage, &inerComp, &leafComp);
			tree.commit();
			tran.commit();

			storage.close();
		}
		{
			embDB::CStorage storage(alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false, false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();

			embDB::CConsolLogger log;
			log.AddRef();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1, nTranCache);
			tran.begin();
			tran.SetLogger(&log);
			insertINBTreeSet <TBtree, TTran, TKey, TValue>(nPageSize, nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc, nTreeRootPage, bCheckCRC);
			std::cout << "File Size " << storage.getFileSize() << std::endl;
			storage.close();
 
		}

		{
			embDB::CStorage storage(alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false, false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			embDB::CConsolLogger log;
			log.AddRef();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1, nTranCache);
			tran1.begin();
			tran1.SetLogger(&log);

			searchINBTreeSet <TBtree, TTran, TKey, TKey>(nPageSize, nCacheBPTreeSize, nCount, 0, nStep, &tran1, alloc, nTreeRootPage, bCheckCRC);
			tran1.commit();
			storage.close();
		}
	
		int64 nRemoveCnt = nCount/2;
		 
		{

			embDB::CStorage storage(alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false, false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			embDB::CConsolLogger log;
			log.AddRef();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1, nTranCache);
			tran.begin();
			tran.SetLogger(&log);
			removeFromBTreeSet <TBtree, TTran, TKey>(nPageSize, nCacheBPTreeSize, nRemoveCnt, 0, nStep, &tran, alloc, nTreeRootPage, bCheckCRC);
			std::cout << "File Size " << storage.getFileSize() << std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage(alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false, false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			embDB::CConsolLogger log;
			log.AddRef();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1, nTranCache);
			tran1.begin();
			tran1.SetLogger(&log);

			searchINBTreeSet <TBtree, TTran, TKey, TKey>(nPageSize, nCacheBPTreeSize, nRemoveCnt, nCount, nStep, &tran1, alloc, nTreeRootPage, bCheckCRC);
			tran1.commit();
			storage.close();
		}
		return;
		{
			embDB::CStorage storage(alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false, false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();

			embDB::CConsolLogger log;
			log.AddRef();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1, nTranCache);
			tran.begin();
			tran.SetLogger(&log);
			insertINBTreeSet <TBtree, TTran, TKey, TValue>(nPageSize, nCacheBPTreeSize, nRemoveCnt, 0, nStep, &tran, alloc, nTreeRootPage, bCheckCRC);
			std::cout << "File Size " << storage.getFileSize() << std::endl;
			storage.close();

		}

		{
			embDB::CStorage storage(alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false, false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			embDB::CConsolLogger log;
			log.AddRef();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1, nTranCache);
			tran1.begin();
			tran1.SetLogger(&log);

			searchINBTreeSet <TBtree, TTran, TKey, TKey>(nPageSize, nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage, bCheckCRC);
			tran1.commit();
			storage.close();
		}
	}
}


void TestBPSetPlusTree()
{
	int64 nCount = 10000000;
	size_t nPageSize = 256;
	uint32 nTranCache = 10;

	testBPTreeSetImpl<TBPSet64, embDB::CTransaction, int64, int64>(nCount, nTranCache, nPageSize,  50, 1, true);



}