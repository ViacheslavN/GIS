#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPMapv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/BPVectorNoPod.h"
#include "../../EmbDB/CompositeIndex.h"
#include "../../EmbDB/CompIndexInnerCompressor.h"
#include "../../EmbDB/CompIndexLeafCompressor.h"
#include "../../EmbDB/CompressCompIndexParams.h"
#include "../../EmbDB/VariantField.h"
#include "CommonLibrary/DebugTime.h"

 
typedef embDB::BPTreeLeafNodeMapv2<embDB::CompositeIndexKey, uint64, embDB::IDBTransactions,
	embDB::BPLeafCompIndexCompressor<uint64>, embDB::TBPVectorNoPOD<embDB::CompositeIndexKey> > TLeafNode;

typedef embDB::BPTreeInnerNodeSetv2<embDB::CompositeIndexKey, embDB::IDBTransactions,
	embDB::BPInnerCompIndexCompressor, embDB::TBPVectorNoPOD<embDB::CompositeIndexKey> > TInnerNode;

typedef embDB::TBPMapV2<embDB::CompositeIndexKey, uint64, embDB::comp<embDB::CompositeIndexKey>, embDB::IDBTransactions,
	embDB::BPInnerCompIndexCompressor, embDB::BPLeafCompIndexCompressor<uint64>, TInnerNode, TLeafNode> TBPTree;



int64 CreateCompParams(embDB::IDBTransactions* pTran)
{
	embDB::CompIndexParams compParmas;
	embDB::FilePagePtr pPage = pTran->getNewPage();
	if(!pPage.get())
		return -1;

	int64 nCompRootPage = pPage->getAddr();
	compParmas.setRootPage(nCompRootPage);
	compParmas.addParams(embDB::ftInteger64);
	compParmas.addParams(embDB::ftInteger64);
	compParmas.save(pTran);
	return nCompRootPage;
	

}
enum eTestType
{
	eCREATE = 1,
	eINSERT = 2,
	eFIND = 4,
	eDELETE = 8
};



template<class Tran>
void insertINDBTreeCompKey  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBPTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	bool bFind = false;
	embDB::TFieldINT64 valint64;
	if(nStart < nEndStart)
	{

		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			embDB::CompositeIndexKey key(pAlloc);
			valint64.setVal(i);
		 
			key.addValue(&valint64);
			key.addValue(&valint64);

			if(!tree.insert(key, i))
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
			embDB::CompositeIndexKey key(pAlloc);
			valint64.setVal(i);

			key.addValue(&valint64);
			key.addValue(&valint64);

			if(!tree.insert(key, i))
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
	//nTreeRootPage = tree.getPageBTreeInfo();
	pTran->OutDebugInfo();


	std::cout << "Insert end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;

}


template<class TTran>
void testCompIndex (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, 
					int32 nCacheBPTreeSize, int32 nTranCache, uint32 type)
{
	CommonLib::alloc_t *pAlloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout <<"Begin test Count: " << nCount << " PageSize :" << nPageSize << std::endl;
	int64 nTreeRootPage = -1;
	int64 nStorageInfoPage = 0;
	int64 nStep = 1;
	if(type & eCREATE)
	{
		embDB::CStorage storage( pAlloc, nCacheStorageSize);
		storage.open(L"d:\\dbplusCompKey.data", false, false,  true, false, nPageSize);
		embDB::FilePagePtr pPage = storage.getNewPage();
		nStorageInfoPage = pPage->getAddr();
		storage.initStorage(pPage->getAddr());
		pPage.release();
		storage.saveStorageInfo();

		TTran tran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1, nTranCache);
		tran.begin();
		int64 nCompParams = CreateCompParams(&tran);
		TBPTree tree(nTreeRootPage, &tran, pAlloc, nCacheBPTreeSize);
		tree.setCompPageInfo(nCompParams, nCompParams);
		tree.saveBTreeInfo(); 
		nTreeRootPage = tree.getPageBTreeInfo();
		tree.commit();
		tran.commit();
	}

	{
		embDB::CStorage storage( pAlloc, nCacheStorageSize);
		storage.open(L"d:\\dbplusCompKey.data", false, false,  false, false, nPageSize);
		storage.setStoragePageInfo(nStorageInfoPage);
		storage.loadStorageInfo();
		TTran tran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran2.data", &storage, 1);
		tran.begin();
		insertINDBTreeCompKey <TTran>(nCacheBPTreeSize, 0, nCount, nStep, &tran, pAlloc, nTreeRootPage);
		storage.close();
	}

	delete pAlloc;


}


void testCompKey()
{
	int64 nCount = 1000000;
	size_t nPageSize = 8192;
	testCompIndex<embDB::CDirectTransactions>(nCount, nPageSize, 10000, 10, 10000, eCREATE|eINSERT|eFIND|eDELETE);
}
