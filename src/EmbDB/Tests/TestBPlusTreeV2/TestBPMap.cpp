#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPMapv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/BaseInnerNodeDIffCompress.h"
#include "../../EmbDB/BaseLeafNodeCompDiff.h"
#include "../../EmbDB/BaseInnerNodeDIffCompress2.h"
#include "../../EmbDB/BaseValueDiffCompressor.h"
#include "../../EmbDB/SignedNumLenDiffCompress.h"




typedef embDB::TBaseValueDiffCompress<int64, int64,embDB::SignedDiffNumLenCompressor64i> TInnerLinkCompress;

typedef embDB::TBPMapV2 <int64,  uint64, embDB::comp<uint64>, embDB::IDBTransaction, 
	embDB::TBPBaseInnerNodeDiffCompressor2<int64, embDB::OIDCompressor, TInnerLinkCompress>, 

	embDB::TBaseLeafNodeDiffComp<int64, uint64, embDB::IDBTransaction, embDB::OIDCompressor> > TBInt64Map;
/*

typedef embDB::TBPMapV2 <int64,  uint64, embDB::comp<uint64>, embDB::IDBTransaction, 
	embDB::TBPBaseInnerNodeDiffCompressor<int64, embDB::OIDCompressor, embDB::InnerLinkCompress>, 
	
	embDB::TBaseLeafNodeDiffComp<int64, uint64, embDB::IDBTransaction, embDB::OIDCompressor> > TBInt64Map;*/


//embDB::TBaseLeafNodeComp<int64, uint64, embDB::IDBTransaction, embDB::OIDCompressor>
//typedef embDB::TBPMapV2 <int64,  uint64, embDB::comp<uint64>, embDB::IDBTransaction> TBInt64Map;

template<class TBtree, class Tran, class TKey, class TValue>
void insertINBTreeMap  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bCheckCRC)
{
	std::cout << "Insert Test"  << std::endl;

	embDB::CBPTreeStatistics statInfo;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192, false, bCheckCRC);
	tree.loadBTreeInfo(); 
	if(nStart < nEndStart)
		tree.SetMinSplit(true);
	tree.SetBPTreeStatistics(&statInfo);
	time.start();
	int64 n = 0;
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{

			if(!tree.insert(TKey(i), TValue(i)))
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
			if(!tree.insert(TKey(i), TValue(i)))
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


	std::cout << "Insert end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;


	std::cout << "Create Inner Nodes: " << statInfo.GetCreateNode(false) << " Create Leaf Nodes: " <<  statInfo.GetCreateNode(true) <<	std::endl;
	std::cout << "Load Inner Nodes: " << statInfo.GetLoadNode(false) << " Load Leaf Nodes: " <<  statInfo.GetLoadNode(true) <<	std::endl;
	std::cout << "Save Inner Nodes: " << statInfo.GetSaveNode(false) << " Save Leaf Nodes: " <<  statInfo.GetSaveNode(true) <<	std::endl;

}




template<class TBtree, class Tran, class TKey, class TValue>
void searchINBTreeMap  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bCheckCRC)
{
	std::cout << "Search Test"  << std::endl;
	embDB::CBPTreeStatistics statInfo;
	CommonLib::TimeUtils::CDebugTime time;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192, false, bCheckCRC);
	tree.loadBTreeInfo();
	tree.SetBPTreeStatistics(&statInfo);
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
			else if(it.key() != it.value())
			{
				std::cout << "Key not EQ value " << i << std::endl;
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
			else if( i != it.key())
			{
				std::cout << "Key not EQ " << i << std::endl;
				nNotFound++;
			}
			else if(it.key() != it.value())
			{
				std::cout << "Key not EQ value " << i << std::endl;
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
	std::cout << "Load Inner Nodes: " << statInfo.GetLoadNode(false) << " Load Leaf Nodes: " <<  statInfo.GetLoadNode(true) <<	std::endl;

}



template<class TBtree, class Tran, class TKey>
void removeFromBTreeMap  (int32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage, bool bCheckCRC)
{
	std::cout << "Remove Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmRemove = 0;
	double treeCom = 0;
	double tranCom  = 0;
	embDB::CBPTreeStatistics statInfo;
	TBtree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192, false ,bCheckCRC);
	tree.loadBTreeInfo();
	tree.SetBPTreeStatistics(&statInfo);
	time.start();
	int64 i = nStart;
	int64 n = 0;

	
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (; i < nEndStart; ++i)
		{	


		
			/*if(i == 3632988)
			{

				TBtree::iterator it = tree.find(TKey(3632989));
				if(it.isNull())
				{
					int dd = 0;
					dd++;
				}
			}*/
 

			if(!tree.remove(TKey(i)))
			{
				std::cout << "Error remove,  not found " << i << std::endl;
			}
			n++;
			/*if(i == 3632988)
			{

				TBtree::iterator it = tree.find(TKey(3632989));
				if(it.isNull())
				{
					int dd = 0;
					dd++;
				}
			}*/
			

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
	
	
	std::cout << "Create Inner Nodes: " << statInfo.GetCreateNode(false) << " Create Leaf Nodes: " <<  statInfo.GetCreateNode(true) <<	std::endl;
	std::cout << "Load Inner Nodes: " << statInfo.GetLoadNode(false) << " Load Leaf Nodes: " <<  statInfo.GetLoadNode(true) <<	std::endl;
	std::cout << "Save Inner Nodes: " << statInfo.GetSaveNode(false) << " Save Leaf Nodes: " <<  statInfo.GetSaveNode(true) <<	std::endl;
	std::cout << "Remove Inner Nodes: " << statInfo.GetDeleteNode(false) << " Remove Leaf Nodes: " <<  statInfo.GetDeleteNode(true) <<	std::endl;


}



template<class TBtree,  class TTran, class TKey, class TValue>
void testBPTreeMapImpl (int64 nCount, size_t nPageSize, int32 nCacheStorageSize, int32 nCacheBPTreeSize, bool bCheckCRC)
{

	typedef typename TBtree::TInnerCompressorParams TInnerCompressorParams;
	typedef typename TBtree::TLeafCompressorParams TLeafCompressorParams;

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
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false,  true, false);
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

				tran.commit();
			}

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran1.data", &storage, 1);
			tran.begin();
			insertINBTreeMap <TBtree, TTran,  TKey, TValue>(nCacheBPTreeSize, nCount, 0, nStep, &tran, alloc, nTreeRootPage,  bCheckCRC);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran2.data", &storage, 1);
			tran1.begin();
			searchINBTreeMap <TBtree, TTran, TKey, TValue>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage,  bCheckCRC);
			storage.close();
		}
		
/*
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran3.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, true);
			storage.close();
		}
		return;*/
	/*	{

			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran4.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, false);
		}*/
		//int64 mRemConst =246762;
		int64 mRemConst =nCount/2;
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran remtran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran5.data", &storage, 1);
			remtran.begin();
			removeFromBTreeMap <TBtree, TTran, TKey>(nCacheBPTreeSize, mRemConst, nCount, nStep, &remtran, alloc, nTreeRootPage,  bCheckCRC);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}
	
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran6.data", &storage, 1);
			tran1.begin();
			searchINBTreeMap <TBtree, TTran, TKey, TValue>(nCacheBPTreeSize, 0, mRemConst, nStep, &tran1, alloc, nTreeRootPage,  bCheckCRC);
			storage.close();
		}
	
		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tran7.data", &storage, 1);
			tran.begin();
			insertINBTreeMap <TBtree, TTran,  TKey, TValue>(nCacheBPTreeSize, mRemConst, nCount, nStep, &tran, alloc, nTreeRootPage,  bCheckCRC);
			std::cout << "File Size " << storage.getFileSize() <<	std::endl;
			storage.close();
		}

		{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.AddRef();
			storage.open(L"d:\\dbplus.data", false, false,  false, false);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran1(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran8.data", &storage, 1);
			tran1.begin();
			searchINBTreeMap <TBtree, TTran, TKey, TValue>(nCacheBPTreeSize, 0, nCount, nStep, &tran1, alloc, nTreeRootPage,  bCheckCRC);
			storage.close();
		}
		/*{
			embDB::CStorage storage( alloc, nCacheStorageSize);
			storage.open("d:\\dbplus.data", false, false,  false, false, nPageSize);
			storage.setStoragePageInfo(nStorageInfoPage);
			storage.loadStorageInfo();
			TTran tran5(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tran9.data", &storage, 1);
			tran5.begin();
			testOrderINBTreeSet <TBtree, TTran, TKey>(nCacheBPTreeSize,  nStep, &tran5, alloc, nTreeRootPage, true);
			storage.close();
		}*/
	}
 
}





void TestBRteeMap()
{
	//__int64 nCount = 1531;

	//__int64 nCount = 6748900;
	int64 nCount = 1000000;
		size_t nPageSize = 8192;

	testBPTreeMapImpl<TBInt64Map,  embDB::CTransaction, int64, int64>(nCount, nPageSize, 50, 2, false);
}