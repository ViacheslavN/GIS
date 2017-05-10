#include "stdafx.h"
#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BTreePlusv3/BaseBPSetv3.h"
 

#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "CommonLibrary/DebugTime.h"

typedef embDB::TBPlusTreeSetV3<int64, embDB::comp<int64>, embDB::IDBTransaction> TBPSet64;




template<class TBtree, class TTran, class TKey, class TValue>
void testBPTreeSetImpl(int64 nCount, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize, bool bCheckCRC)
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
		embDB::CStorage storage(alloc, nCacheStorageSize);
		storage.AddRef();
		storage.open(L"d:\\dbplus.data", false, false, true, false);
		embDB::FilePagePtr pPage = storage.getNewPage(nPageSize);
		nStorageInfoPage = pPage->getAddr();
		storage.initStorage(pPage->getAddr());
		//pPage.release();
		storage.saveStorageInfo();

		{
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\db\\createtran.data", &storage, 1);
			tran.begin();
			embDB::FilePagePtr pPage = tran.getNewPage(256);

			nTreeRootPage = pPage->getAddr();
			TBtree tree(-1, &tran, alloc, nCacheBPTreeSize, 8192, false, bCheckCRC);
			TInnerCompressorParams inerComp;
			TLeafCompressorParams leafComp;
			tree.init(nTreeRootPage, &inerComp, &leafComp);
			tree.commit();
			tran.commit();
		}
		{
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1);
			tran.begin();
			insertINBTreeMap <TBtree, TTran, TKey, TValue>(nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc, nTreeRootPage, bCheckCRC);
			std::cout << "File Size " << storage.getFileSize() << std::endl;
			storage.close();
		}
		
	}
}



template<class TBtree, class Tran, class TKey, class TValue>
void insertINBTreeMap(int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bCheckCRC)
{
	std::cout << "Insert Test" << std::endl;
 
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192, false, bCheckCRC);
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



void TestBPSetPlusTree()
{
	int64 nCount = 1000000;
	size_t nPageSize = 8192;

	testBPTreeSetImpl<TBPSet64, embDB::CTransaction, int64, int64>(nCount, nPageSize, 50, 3, false);



}