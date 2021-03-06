#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPSetv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/BaseInnerNodeDIffCompress.h"
#include "../../EmbDB/MultiIndexBase.h"
#include "../../EmbDB/MultiIndexBPTree.h"
#include "../../EmbDB/BPMultiIndexLeafNodeCompressor.h"
#include "../../EmbDB/BPMultiInnerIndexNodeCompressor.h"
#include "../../EmbDB/MultiKeyCompressor.h"

typedef embDB::IndexTuple<int64> TIndexTuple;
typedef embDB::MultiIndexBaseComp<int64> TComp;

typedef embDB::TMultiKeyCompressor<int64, int64, embDB::SignedDiffNumLenCompressor264i> TMultiKeyCompressor;
 


typedef embDB::BPMultiIndexInnerNodeCompressor<int64, embDB::IDBTransaction, TMultiKeyCompressor> TInnerCompressor;
typedef embDB::BPLeafNodeMultiIndexCompressor<int64, embDB::IDBTransaction, TMultiKeyCompressor>	TLeafCompressor;


typedef embDB::TBPSetV2<TIndexTuple, TComp, embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;
typedef TBTree::TInnerCompressorParams TInnerCompressorParams;
typedef TBTree::TLeafCompressorParams TLeafCompressorParams;




template<class TBtree, class Tran, class TKey>
void insertInMultiIndex  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
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
		for (int64 i = nStart; i < nEndStart; ++i)
		{

			if(!tree.insert(TKey(i, i)))
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



			if(!tree.insert(TKey(i, i)))
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
void searchInMultiIndex  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo();
	int64 nNotFound = 0;
	double searchTm  = 0;
	int64 n = 0;
	time.start();


	TBtree::iterator it = tree.find(TKey(nStart, nStart));
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
			it = tree.find(TKey(i, i), &it,  true);
			if(it.isNull())
			{
				std::cout << "Not found " << i << std::endl;
				nNotFound++;
			}
			else if( i != it.key().m_key)
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
			TBtree::iterator it = tree.find(TKey(i, i));
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
void removeFromMultiIndex  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
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


		
			

			if(!tree.remove(TKey(i, i)))
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}
			n++;

			

			TBtree::iterator it = tree.find(TKey(i, i));
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
			
			if(!tree.remove(TKey(i, i)))
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}
						
			TBtree::iterator it = tree.find(TKey(i, i));
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
	//std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;
}


template<class TBtree,  class TTran, class TKey>
void testMuiltiIndexImpl (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize, int32 nTranCache)
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

				TInnerCompressorParams inerComp;
				TLeafCompressorParams leafComp;
				tree.init(nTreeRootPage, &inerComp, &leafComp); 
				tran.commit();
			}

	
	 


			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1, nTranCache);
			tran.begin();
			insertInMultiIndex <TBtree, TTran,  TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() << " StorageInfoPage " 
				<<  nStorageInfoPage << " nTreeRootPage " <<nTreeRootPage  << std::endl;
			storage.close();
		}

		
	
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchInMultiIndex <TBtree, TTran, TKey>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage);
			storage.close();
		}
		int64 mRemConst =nCount/2;

		{

			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran5.data", &storage, 1, nTranCache);
			remtran.begin();
			removeFromMultiIndex <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, 0, nStep, &remtran, alloc, nTreeRootPage);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchInMultiIndex <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, nCount, nStep, &tran1, alloc, nTreeRootPage);
			storage.close();
		}
	}
 
}



void TestMuiltiIndex()
{

	__int64 nCount = 1000000;
	size_t nPageSize = 8192;
	testMuiltiIndexImpl<TBTree, embDB::CDirectTransaction, TIndexTuple>(nCount, nPageSize, 10000, 10, 10000);
}