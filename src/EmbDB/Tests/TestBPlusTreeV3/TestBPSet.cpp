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

			tree.insert(34);

			tree.commit();
			tran.commit();
		}

		
	}
}


void TestBPSetPlusTree()
{
	int64 nCount = 1000000;
	size_t nPageSize = 8192;

	testBPTreeSetImpl<TBPSet64, embDB::CTransaction, int64, int64>(nCount, nPageSize, 50, 2, false);



}