#include "stdafx.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include "../../EmbDB/RectSpatialBPMapTree.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/DebugTime.h"
#include <iostream>
#include <set>

#define  INSERT 1
#define  SEARCH 2
#define  IDENTIFY 3
void TestRectZorder(int Xmax, int Ymax, int qXmin, int qYmin, int qXmax, int qYmax);
typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU16, uint64,
	embDB::ZPointComp<embDB::ZOrderRect2DU16> 
> TBPMapRect16;

typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU32, uint64,
	embDB::ZRect32Comp, embDB::IDBTransactions,
embDB::BPSpatialRectInnerNodeSimpleCompressor<embDB::ZOrderRect2DU32 >,	
embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU32, uint64> > TBPMapRect32;



 
/*typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU64, uint64,
	embDB::ZRect64Comp, embDB::IDBTransactions,
	embDB::BPSpatialRectInnerNodeSimpleCompressor<embDB::ZOrderRect2DU64>,	
	embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU64, uint64 > > TBPMapRect64;*/


 


template <class TSparialTree, class TCoodType, class Tran>
void TestRectSpatialInsert(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	CommonLib::TRect2D<TCoodType>         ExtentTree;
	ExtentTree.m_minX = 0;
	ExtentTree.m_minY = 0;
	ExtentTree.m_maxX =  (TCoodType)nEndStart;
	ExtentTree.m_maxY =  (TCoodType)nEndStart;
	TSparialTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	tree.setExtent(ExtentTree);
	time.start();
	uint64 nOID = 1;

	uint64 nCount = (nEndStart - nStart)/nStep;
	nCount *= nCount;
	int64 nProcStep = nCount/100;
	if(!nProcStep)
		nProcStep = 1000000;

	CommonLib::TRect2D<TCoodType>         feature;
	for (uint64 x = nStart; x < nEndStart - nStep; x +=nStep)
	{
		for (uint64 y = nStart; y < nEndStart - nStep; y +=nStep)
		{

			tree.insert((TCoodType)x, (TCoodType)y, (TCoodType)x + (TCoodType)nStep, (TCoodType)y + (TCoodType)nStep, nOID);
			feature.set((TCoodType)x, (TCoodType)y, (TCoodType)x + (TCoodType)nStep, (TCoodType)y + (TCoodType)nStep );
			if(nOID%nProcStep == 0)
			{
				std::cout << nOID  << "  " << (nOID* 100)/nCount << " %" << '\r';
			}
			++nOID;

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



template <class TSparialTree, class TCoodType, class Tran, class TZorderType>
void TestRectSpatialSearchByQuery(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, 
	TCoodType qXmin,  TCoodType qYmin,  TCoodType qXmax,  TCoodType qYmax, 
	Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	CommonLib::TRect2D<TCoodType>         ExtentTree;
	CommonLib::TRect2D<TCoodType>         SpatialQuery;
	CommonLib::TRect2D<TCoodType>         rectFeature;
	CommonLib::TRect2D<TCoodType>         ZrectFeature;

 
	ExtentTree.m_minX = 0;
	ExtentTree.m_minY = 0;
	ExtentTree.m_maxX = (TCoodType)nEndStart;
	ExtentTree.m_maxY =  (TCoodType)nEndStart;
	TSparialTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	tree.setExtent(ExtentTree);
	time.start();
	uint64 nNotFound = 0;
	uint64 nOIDErr = 0;
	double searchTm  = 0;
	time.start();

	int64 nCount = (nEndStart - nStart)/nStep;
	nCount *= nCount;
	if(!nCount)
		nCount = 1;
	int64 nProcStep = nCount/100;
	if(!nProcStep)
		nProcStep = 1;
	int64 nCnt = 0;
	int64 nCntNotQuery = 0;
	int64 nCntInQuery = 0;
	int64 nDubble = 0;
 
	double searchZorderTm  = 0;

	int64 nCntFull = 0;
	int64 nCntNotQueryFull  = 0;
	int64 nCntInQueryFull  = 0;
	int64 nDubbleFull = 0;

	double searchFullTm  = 0;

	int64 nDouble = 0;
	TZorderType zVal;
	TZorderType zValMax;
	TCoodType xMin, yMin, xMax, yMax;
  
	TZorderType zFullVal;

	{
		SpatialQuery.m_minX = (TCoodType)qXmin;
		SpatialQuery.m_maxX = (TCoodType)qXmax;
		SpatialQuery.m_minY = (TCoodType)qYmin;
		SpatialQuery.m_maxY = (TCoodType)qYmax;
	
	 
		
			{
				nCnt = 0;
				nCntNotQuery = 0;
				nCntInQuery = 0;
	 
				time.start();
				TSparialTree::TSpatialIterator it = tree.spatialQuery(SpatialQuery.m_minX, SpatialQuery.m_minY, SpatialQuery.m_maxX, SpatialQuery.m_maxY);
				if(it.isNull())
				{
					std::cout << "Not found poins for query Xmin: " << SpatialQuery.m_minX <<" Ymin: " << SpatialQuery.m_minY << " xMax: " <<  SpatialQuery.m_maxX << " yMax: " <<  SpatialQuery.m_maxY << std::endl;
					nNotFound++;
				}
				else
				{

					TZorderType zLast;
					int ff = 0;
					while(!it.isNull())
					{
						++nCnt;

						zVal = it.key();
					
						int64 nOID = it.value();
						//	if(zVal > zValMax)
						//		break;
					
						 
										
						if(nCnt%nProcStep == 0)
						{
							std::cout <<"zOrder Cnt:  " << nCnt << " " << (nCnt* 100)/nCount << " %" << '\r';
						}

						zVal.getXY(xMin, yMin, xMax, yMax);
						ZrectFeature.set(xMin, yMin, xMax, yMax);
						if(!SpatialQuery.isIntersection(ZrectFeature) && !ZrectFeature.isInRect(SpatialQuery))
						{
							++nCntNotQuery;
						}
						else 
						{
 
							++nCntInQuery;
 
						}
						
						if(!it.next())
							break;
					}


				}
					searchZorderTm = time.stop();

			}
			

	
		{
			

			uint64 nCount = (uint64)(0xFFFF - (uint64)SpatialQuery.m_minX )/nStep;
			nCount *= nCount;
			uint64 nProcStep = nCount;
			if(!nProcStep)
				nProcStep = 1000000000;
			nProcStep = 42000000/100;
			zValMax.setZOrder(SpatialQuery.m_maxX, SpatialQuery.m_maxY,  TZorderType::coordMax,  TZorderType::coordMax);

			nCntFull = 0;
			nCntNotQueryFull  = 0;
			nCntInQueryFull  = 0;
			nDubbleFull = 0;
		 
			TSparialTree::iterator it = tree.identify(0, 0, SpatialQuery.m_minX, SpatialQuery.m_minY);
			time.start();

			while(!it.isNull())
			{
	
				zFullVal = it.key();

				int64 nOID = it.value();
				if(zFullVal > zValMax)
					break;
 
				
				++nCntFull;

				if(nCntFull%nProcStep == 0)
				{
					std::cout << "Full Search:  "  << nCntFull << " " << (nCntFull* 100)/nCount << " %" << '\r';
				}

				zFullVal.getXY(xMin, yMin, xMax, yMax);
				rectFeature.set(xMin, yMin, xMax, yMax);
				if(!SpatialQuery.isIntersection(rectFeature) && !rectFeature.isInRect(SpatialQuery))
				{
					++nCntNotQueryFull;
				}
				else
				{
					++nCntInQueryFull;
					if(nCntInQueryFull > 82)
					{
						int d = 0;
						d++;
					}
 
				}

			

				if(!it.next())
					break;
			}
		}
			searchFullTm = time.stop();

		
		
		

			std::cout << "Found Zorder  time: " << searchZorderTm <<" Cnt: " << nCnt << " InQuery " <<  nCntInQuery<< " NotInQuery " << nCntNotQuery <<   std::endl;
			std::cout << "Found Full    time: " << searchFullTm   <<" Cnt: " << nCntFull << " InQuery " <<  nCntInQueryFull<< " NotInQuery " << nCntNotQueryFull  << std::endl;
			
			
			return;
		}


}


template <class TSparialTree, class TCoodType, class Tran, class TZorderType>
void TestRectSpatialSearchByFeature(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	CommonLib::TRect2D<TCoodType>         ExtentTree;
	ExtentTree.m_minX = 0;
	ExtentTree.m_minY = 0;
	ExtentTree.m_maxX = (TCoodType)nEndStart;
	ExtentTree.m_maxY =  (TCoodType)nEndStart;
	TSparialTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	tree.setExtent(ExtentTree);
	time.start();
	uint64 nOID = 1;
	uint64 nNotFound = 0;
	uint64 nOIDErr = 0;
	double searchTm  = 0;
	time.start();

	int64 nCount = (nEndStart - nStart)/nStep;
	nCount *= nCount;
	int64 nProcStep = nCount/100;
	for (uint64 x = nStart; x < nEndStart  - nStep; x +=nStep)
	{
		for (uint64 y = nStart; y < nEndStart  - nStep; y +=nStep)
		{
			TZorderType key((TCoodType)x, (TCoodType)y, (TCoodType)x + (TCoodType)nStep, (TCoodType)y + (TCoodType)nStep);
			TSparialTree::iterator it = tree.find(key);
			if(it.isNull())
			{
				std::cout << "Not found x: " << x <<" y: " << y << std::endl;
				nNotFound++;
			}
			else if(it.value() != nOID)
			{
				std::cout << "OID found Error x: " << x <<" y: " << y << " must be OID: " << nOID << " found OID: "<< it.value() << std::endl;
				nOIDErr++;
			}
			++nOID;
			if(nOID%nProcStep == 0)
			{
				std::cout << nOID  << "  " << (nOID* 100)/nCount << " %" << '\r';
			}

		}

	}




	searchTm = time.stop();
	std::cout << "Search end key start: " << nStart << " key end: " << nEndStart << " Not found: " << nNotFound<< " OID Error found: " << nOIDErr  << " Total time: " << searchTm << std::endl;

}

template <class TSparialTree, class TCoodType, class TTran, class TZorderType>
void TestRectSpatial(const CommonLib::CString& sFileName,  int nCacheStorageSize, int nPageSize, TCoodType nBegin, TCoodType nEnd, TCoodType nStep, TCoodType qXmin,  TCoodType qYmin,  TCoodType qXmax,  TCoodType qYmax,  int nType)
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout <<"Begin test Count: Begin: " << nBegin << " End: " << nEnd << " Step: " << nStep << " PageSize :" << nPageSize << std::endl;
	{
		embDB::CStorage storage( alloc, nCacheStorageSize);
		storage.open(sFileName.cwstr(), false, false,  nType != INSERT ? false : true, false, nPageSize);
	

		int64 nTreeRootPage = nType != INSERT ? 6 : -1;

		if(nType == INSERT)
		{
			embDB::FilePagePtr pPage = storage.getNewPage();
			storage.initStorage(pPage->getAddr());
			storage.saveStorageInfo();

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo.data", &storage, 1);
			tran.begin();
			TestRectSpatialInsert<TSparialTree,TCoodType, TTran>(50, nBegin, nEnd, nStep, &tran, alloc, nTreeRootPage);
			return;
		}
		if(nType == SEARCH)
		{
			storage.setStoragePageInfo(0);
			storage.loadStorageInfo();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranSearch.data", &storage, 1);
			tran.begin();
			TestRectSpatialSearchByQuery<TSparialTree,TCoodType, TTran, TZorderType>(50, nBegin, nEnd, nStep,  qXmin,   qYmin,   qXmax,   qYmax,   &tran, alloc, nTreeRootPage);
			
		}
		if(nType == IDENTIFY)
		{

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranSearch.data", &storage, 1);
			tran.begin();
			TestRectSpatialSearchByFeature<TSparialTree,TCoodType, TTran, TZorderType>(50, nBegin, nEnd, nStep, &tran, alloc, nTreeRootPage);

		}
	}
}

void TestRectSpatialTree()
{
	 TestRectSpatial<TBPMapRect16, uint16, embDB::CDirectTransactions, embDB::ZOrderRect2DU16>("d:\\db\\dbspatialrect16.data", 50, 8192, 0, 1000, 10,  0, 0, 100, 100, INSERT);
	// TestRectSpatial<TBPMapRect16, uint16, embDB::CDirectTransactions, embDB::ZOrderRect2DU16>("d:\\db\\dbspatialrect16.data", 50, 8192, 0, 2000, 10, 333, 444, 999, 1245, SEARCH);
	// TestRectSpatial<TBPMapRect16, uint16, embDB::CDirectTransactions, embDB::ZOrderRect2DU16>("d:\\dbspatialrect16.data", 50, 8192, 0, 0xFFFF,100, 1000, IDENTIFY);
	  TestRectZorder(2000, 2000, 333, 444, 999, 1245);
	//TestRectSpatial<TBPMapRect32Ugd, uint32, embDB::CDirectTransactions, ugdRectKey>("d:\\db\\dbspatialrect32Ugd.data", 50, 8192, 0, 0xFFFF, 10, 0, INSERT);
    //TestRectSpatial<TBPMapRect32Ugd, uint32, embDB::CDirectTransactions, ugdRectKey>("d:\\db\\dbspatialrect32Ugd.data", 50, 8192, 0, 160, 10, 10, SEARCH);

	


	// TestRectSpatial<TBPMapRect32, uint32, embDB::CDirectTransactions, embDB::ZOrderRect2DU32>("d:\\db\\dbspatialrect32.data", 50, 8192,  0 , 0xFFFF, 10, 0, INSERT);
	// TestRectSpatial<TBPMapRect32, uint32, embDB::CDirectTransactions, embDB::ZOrderRect2DU32>("d:\\db\\dbspatialrect32.data", 50, 8192,0, 160,10, 10, SEARCH);
	// TestRectSpatial<TBPMapRect32, uint32, embDB::CDirectTransactions, embDB::ZOrderRect2DU32>("d:\\db\\dbspatialrect32.data", 50, 8192, 0, 0xFFFF,10, 10, IDENTIFY);
	
	//TestRectSpatial<TBPMapRect64, uint64, embDB::CDirectTransactions, embDB::ZOrderRect2DU64>("d:\\db\\dbspatialrect64.data", 50, 8192,   0xFFFFFFFFFFFFFFFF - 60000, 0xFFFFFFFFFFFFFFFF, 10, 0, INSERT);
	//TestRectSpatial<TBPMapRect64, uint64, embDB::CDirectTransactions, embDB::ZOrderRect2DU64>("d:\\db\\dbspatialrect64.data", 50, 8192, 0xFFFFFFFFFFFFFFFF - 60000, 0xFFFFFFFFFFFFFFFF - 50000,10, 10, SEARCH);
	//TestRectSpatial<TBPMapRect64, uint64, embDB::CDirectTransactions, embDB::ZOrderRect2DU64>("d:\\db\\dbspatialrect64.data", 50, 8192, 0, 0xFFFF,10, 10, IDENTIFY);
}