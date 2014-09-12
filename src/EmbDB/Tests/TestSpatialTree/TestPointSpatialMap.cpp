#include "stdafx.h"
//#include "../../EmbDB/PointSpatialBPMaTraits.h"
#include "../../EmbDB/PointSpatialBPMapTree.h"
#include "../../EmbDB/SpatialKey.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/PoinMapInnerCompressor64.h"
#include "../../EmbDB/PoinMapLeafCompressor64.h"
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/DebugTime.h"
#include <iostream>
#include <set>

typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU16, uint64,
	embDB::ZPointComp<embDB::ZOrderPoint2DU16> > TBPMapPOint16;


typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU32, uint64,
	embDB::ZPointComp<embDB::ZOrderPoint2DU32> > TBPMapPOint32;



typedef embDB::TBPPointSpatialMap<
	embDB::ZOrderPoint2DU64, 	uint64,	embDB::ZPointComp64, 
	embDB::BPSpatialPointInnerNodeSimpleCompressor64,
    embDB::BPSpatialPointLeafNodeMapSimpleCompressor64<uint64> 
> TBPMapPOint64;




template <class TSparialTree, class TCoodType, class Tran>
void TestPointSpatialInsert(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64& nTreeRootPage)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	embDB::TRect2D<TCoodType>         ExtentTree;
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
	for (uint64 x = nStart; x < nEndStart; x +=nStep)
	{
		for (uint64 y = nStart; y < nEndStart; y +=nStep)
		{

			tree.insert((TCoodType)x, (TCoodType)y, nOID);
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


template <class TSparialTree, class TCoodType, class Tran>
void TestPointSpatialSearchByFeature(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	embDB::TRect2D<TCoodType>         ExtentTree;
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
	for (uint64 x = nStart; x < nEndStart; x +=nStep)
	{
		for (uint64 y = nStart; y < nEndStart; y +=nStep)
		{
			TSparialTree::iterator it = tree.identify((TCoodType)x, (TCoodType)y);
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


template <class TSparialTree, class TCoodType, class Tran, class TZorderType>
void TestPointSpatialSearchByQuery(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, TCoodType nSearchStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	embDB::TRect2D<TCoodType>         ExtentTree;
	embDB::TRect2D<TCoodType>         SpatialQuery;
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
	int64 nProcStep = nCount/100;
	int64 nCnt = 0;
	int64 nCntNotQuery = 0;
	int64 nCntInQuery = 0;
	int64 nDubble = 0;
	std::set<uint64> m_ODIs;
	double searchZorderTm  = 0;

	int64 nCntFull = 0;
	int64 nCntNotQueryFull  = 0;
	int64 nCntInQueryFull  = 0;
	int64 nDubbleFull = 0;
	std::set<uint64> m_ODIsFull;
	double searchFullTm  = 0;

	int64 nDouble = 0;
	TZorderType zVal;
	TZorderType zValMax;
	TCoodType nx, ny;
	nSearchStep = 1;


	/*for (uint64 x = nStart; x <= nEndStart - nSearchStep; x +=nSearchStep)
	{
		SpatialQuery.m_minX = (TCoodType)x;
		//SpatialQuery.m_minY = (TCoodType)x;
		for (uint64 y = nStart; y <= nEndStart - nSearchStep; y +=nSearchStep)
		{
			SpatialQuery.m_minY = (TCoodType)y;
			//SpatialQuery.m_minX = (TCoodType)y;
			TSparialTree::TSpatialIterator it = tree.spatialQuery(SpatialQuery.m_minX, SpatialQuery.m_minY, SpatialQuery.m_maxX, SpatialQuery.m_maxY);
			//it.findNext(false);
			//it.next();
			TZorderType key(SpatialQuery.m_minX , SpatialQuery.m_maxY);
			
			//TSparialTree::iterator it = tree.find(key);
			it.isNull();
		}
	}*/
	//for (uint64 x = nStart; x <= nEndStart - nSearchStep; x +=nSearchStep)
	{
		SpatialQuery.m_minX = (TCoodType)nStart;
		SpatialQuery.m_maxX = (TCoodType)nEndStart;
	
		//for (uint64 y = nStart; y <= nEndStart - nSearchStep; y +=nSearchStep)
		{
			SpatialQuery.m_minY = (TCoodType)nStart;
			SpatialQuery.m_maxY = (TCoodType)nEndStart;
			uint64 nCount = (uint64)(SpatialQuery.m_maxX - (uint64)SpatialQuery.m_minX );
			nCount *= nCount;
			uint64 nProcStep = nCount/100;
			if(!nProcStep)
				nProcStep = 1000000000;
			zValMax.setZOrder(SpatialQuery.m_maxX, SpatialQuery.m_maxY);
		
			

			{
				nCnt = 0;
				nCntNotQuery = 0;
				nCntInQuery = 0;
				nDubble = 0;
				m_ODIs.clear();
				time.start();
				TSparialTree::TSpatialIterator it = tree.spatialQuery(SpatialQuery.m_minX, SpatialQuery.m_minY, SpatialQuery.m_maxX, SpatialQuery.m_maxY);
				if(it.isNull())
				{
					std::cout << "Not found poins for query Xmin: " << SpatialQuery.m_minX <<" Ymin: " << SpatialQuery.m_minY << " xMax: " <<  SpatialQuery.m_maxX << " yMax: " <<  SpatialQuery.m_maxY << std::endl;
					nNotFound++;
				}
				else
				{

					while(!it.isNull())
					{
						++nCnt;

						zVal = it.key();
						int64 nOID = it.value();
							if(zVal > zValMax)
								break;

						/*if(m_ODIs.find(nOID) != m_ODIs.end())
						{
							nDouble++;
						}
						else
						{
							m_ODIs.insert(nOID);
						}*/

						if(nCnt%nProcStep == 0)
						{
							std::cout <<"zOrder Cnt:  " << nCnt << " " << (nCnt* 100)/nCount << " %" << '\r';
						}

						zVal.getXY(nx, ny);
						if(!SpatialQuery.isPoinInRect(nx, ny))
						{
							++nCntNotQuery;
						}
						else
							++nCntInQuery;

						if(nCnt == 1229)
						{
							int dd = 0;
							dd++;
						}

						if(!it.next())
							break;
					}
					searchZorderTm = time.stop();

				}
			}

			nCntFull = 0;
			nCntNotQueryFull  = 0;
			nCntInQueryFull  = 0;
			nDubbleFull = 0;
			m_ODIsFull.clear();
			TSparialTree::iterator it = tree.identify(SpatialQuery.m_minX, SpatialQuery.m_minY);
			time.start();
			while(!it.isNull())
			{
				zVal = it.key();
				if(zVal > zValMax)
					break;
				++nCntFull;
		 
				if(nCntFull%nProcStep == 0)
				{
					std::cout << "Full Search:  "  << nCntFull << " " << (nCntFull* 100)/nCount << " %" << '\r';
				}

				zVal.getXY(nx, ny);
				if(!SpatialQuery.isPoinInRect(nx, ny))
				{
					++nCntNotQueryFull;
				}
				else
					++nCntInQueryFull;
				it.next();
			}
			searchFullTm = time.stop();
		
		

			std::cout << "Found Zorder  time: " << searchZorderTm <<" Cnt: " << nCnt << " InQuery " <<  nCntInQuery<< " NotInQuery " << nCntNotQuery  << std::endl;
			std::cout << "Found Full    time: " << searchFullTm   <<" Cnt: " << nCntFull << " InQuery " <<  nCntInQueryFull<< " NotInQuery " << nCntNotQueryFull  << std::endl;
			return;
		}

	}




	searchTm = time.stop();
	std::cout << "Search end key start: " << nStart << " key end: " << nEndStart << " Not found: " << nNotFound<< " OID Error found: " << nOIDErr  << " Total time: " << searchTm << std::endl;

}

template <class TSparialTree, class TCoodType, class TTran, class TZorderType>
void TestPointSpatial(const CommonLib::str_t& sFileName,  int nCacheStorageSize, int nPageSize, TCoodType nBegin, TCoodType nEnd, TCoodType nStep, TCoodType nSearchStep, bool onlySearch = false)
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout <<"Begin test Count: Begin: " << nBegin << " End: " << nEnd << " Step: " << nStep << " PageSize :" << nPageSize << std::endl;
	{
		embDB::CStorage storage( alloc, nCacheStorageSize);
		storage.open(sFileName, false, false,  onlySearch ? false : true, false, nPageSize);
		embDB::CFilePage* pPage = storage.getNewPage();
		storage.setStoragePageInfo(pPage->getAddr());
		storage.saveStorageInfo();

		int64 nTreeRootPage = onlySearch ? 3 : -1;

		if(!onlySearch)
		{
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo.data", &storage, 1);
			tran.begin();
			TestPointSpatialInsert<TSparialTree,TCoodType, TTran>(50, nBegin, nEnd, nStep, &tran, alloc, nTreeRootPage);
			return;
		}

		/*{

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranSearch.data", &storage, 1);
			tran.begin();
			TestPointSpatialSearchByFeature<TSparialTree,TCoodType, TTran>(50, nBegin, nEnd, nStep, &tran, alloc, nTreeRootPage);
			
		}*/
		{

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo1.data", &storage, 1);
			tran.begin();
			TestPointSpatialSearchByQuery<TSparialTree,TCoodType, TTran, TZorderType>(50, nBegin, nEnd, nStep, nSearchStep,  &tran, alloc, nTreeRootPage);

		}
	}
}
void TestPointSpatialTree()
{
	//TestPointSpatial<TBPMapPOint16, uint16, embDB::CDirectTransactions, embDB::ZOrderPoint2DU16>("d:\\dbspatialpointFor.data", 50, 8192, 10000-100, 10000, 1, 1);
	//TestPointSpatial<TBPMapPOint16, uint16, embDB::CDirectTransactions, embDB::ZOrderPoint2DU16>("d:\\dbspatialpointFor.data", 50, 8192, 10000-100, 10000, 1000, 100, true);


	//TestPointSpatial<TBPMapPOint32, uint32, embDB::CDirectTransactions, embDB::ZOrderPoint2DU32>("d:\\dbspatialpoint32.data", 50, 8192, 0, 10000, 1, 1000);
//	TestPointSpatial<TBPMapPOint32, uint32, embDB::CDirectTransactions, embDB::ZOrderPoint2DU32>("d:\\dbspatialpoint32.data", 50, 8192, 1000, 3000, 1,  500, true);

	//TestPointSpatial<TBPMapPOint64, uint64, embDB::CDirectTransactions, embDB::ZOrderPoint2DU64>("d:\\dbspatialpoint64.data", 50, 8192, 0xFFFFFFFFFFFFFFFF-1000, 0xFFFFFFFFFFFFFFFF, 1, 1000);
  //  TestPointSpatial<TBPMapPOint64, uint64, embDB::CDirectTransactions, embDB::ZOrderPoint2DU64>("d:\\dbspatialpoint64.data", 50, 8192, 0, 0xFFFFFFFFFFFFFFFF, 1,  500, true);
}