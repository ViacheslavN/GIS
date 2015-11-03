#include "stdafx.h"
//#include "../../EmbDB/PointSpatialBPMaTraits.h"
#include "../../EmbDB/PointSpatialBPMapTree.h"
#include "CommonLibrary/SpatialKey.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/PoinMapInnerCompressor64.h"
#include "../../EmbDB/PoinMapLeafCompressor64.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/DebugTime.h"
#include <iostream>
#include <set>

typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU16, uint64,
	embDB::ZPointComp<embDB::ZOrderPoint2DU16> > TBPMapPOint16;


typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU32, uint64,
	embDB::ZPointComp<embDB::ZOrderPoint2DU32> > TBPMapPOint32;



typedef embDB::TBPPointSpatialMap<
	embDB::ZOrderPoint2DU64, 	uint64,	embDB::ZPointComp64, 
	embDB::IDBTransaction,
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
void TestPointSpatialSearchByQuery(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep,
			TCoodType qXmin,  TCoodType qYmin,  TCoodType qXmax,  TCoodType qYmax,
			Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	CommonLib::TRect2D<TCoodType>         ExtentTree;
	CommonLib::TRect2D<TCoodType>         SpatialQuery;
	ExtentTree.m_minX = 0;
	ExtentTree.m_minY = 0;
	ExtentTree.m_maxX = (TCoodType)nEndStart;
	ExtentTree.m_maxY =  (TCoodType)nEndStart;
	TSparialTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	tree.setExtent(ExtentTree);
	time.start();

	 
	int64 nCnt = 0;
	int64 nCntNotQuery = 0;
	int64 nCntInQuery = 0;
	double searchZorderTm  = 0;

	int64 nCntFull = 0;
	int64 nCntNotQueryFull  = 0;
	int64 nCntInQueryFull  = 0;
	double searchFullTm  = 0;


	SpatialQuery.m_minX = (TCoodType)qXmin;
	SpatialQuery.m_maxX = (TCoodType)qXmax;
	SpatialQuery.m_minY = (TCoodType)qYmin;
	SpatialQuery.m_maxY = (TCoodType)qYmax;

	TZorderType zValMax, zVal;


	uint64 nCountX = (uint64)(SpatialQuery.m_maxX - (uint64)SpatialQuery.m_minX );
	uint64 nCountY = (uint64)(SpatialQuery.m_maxY - (uint64)SpatialQuery.m_minY);
	uint64 nCount = (nCountX * nCountY)/nStep;
	

	TCoodType nx, ny;
	uint64 nProcStep = nCount/100;
	if(!nProcStep)
		nProcStep = 1000000000;
	zValMax.setZOrder(SpatialQuery.m_maxX, SpatialQuery.m_maxY);
		
			

	{
				nCnt = 0;
				nCntNotQuery = 0;
				nCntInQuery = 0;
					time.start();
				TSparialTree::TSpatialIterator it = tree.spatialQuery(SpatialQuery.m_minX, SpatialQuery.m_minY, SpatialQuery.m_maxX, SpatialQuery.m_maxY);
				if(it.isNull())
				{
					std::cout << "Not found poins for query Xmin: " << SpatialQuery.m_minX <<" Ymin: " << SpatialQuery.m_minY << " xMax: " <<  SpatialQuery.m_maxX << " yMax: " <<  SpatialQuery.m_maxY << std::endl;
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

						
						zVal.getXY(nx, ny);
						if(!SpatialQuery.isPoinInRect(nx, ny))
						{
							++nCntNotQuery;
						}
						else
							++nCntInQuery;


						if(nCntInQuery%nProcStep == 0)
						{
							std::cout <<"zOrder Cnt:  " << nCntInQuery << " " << (nCntInQuery* 100)/nCount << " %" << '\r';
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

			TSparialTree::iterator it = tree.identify(SpatialQuery.m_minX, SpatialQuery.m_minY);
			time.start();
			while(!it.isNull())
			{
				zVal = it.key();
				if(zVal > zValMax)
					break;
				++nCntFull;
		 
			

				zVal.getXY(nx, ny);
				if(!SpatialQuery.isPoinInRect(nx, ny))
				{
					++nCntNotQueryFull;
				}
				else
					++nCntInQueryFull;

				if(nCntInQueryFull%nProcStep == 0)
				{
					std::cout << "Full Search:  "  << nCntInQueryFull << " " << (nCntInQueryFull* 100)/nCount << " %" << '\r';
				}

				it.next();
			}
			searchFullTm = time.stop();
		
		

	std::cout << "Found Zorder  time: " << searchZorderTm <<" Cnt: " << nCnt << " InQuery " <<  nCntInQuery<< " NotInQuery " << nCntNotQuery  << std::endl;
	std::cout << "Found Full    time: " << searchFullTm   <<" Cnt: " << nCntFull << " InQuery " <<  nCntInQueryFull<< " NotInQuery " << nCntNotQueryFull  << std::endl;
	return;

}

template <class TSparialTree, class TCoodType, class TTran, class TZorderType>
void TestPointSpatial(const CommonLib::CString& sFileName,  int nCacheStorageSize, int nPageSize, 
			TCoodType nBegin, TCoodType nEnd, TCoodType nStep, 
			TCoodType qXmin,  TCoodType qYmin,  TCoodType qXmax,  TCoodType qYmax, 
			bool onlySearch = false)
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout <<"Begin test Count: Begin: " << nBegin << " End: " << nEnd << " Step: " << nStep << " PageSize :" << nPageSize << std::endl;
	{
		embDB::CStorage storage( alloc, nCacheStorageSize);
		storage.open(sFileName.cwstr(), false, false,  onlySearch ? false : true, false, nPageSize);
	

		int64 nTreeRootPage = onlySearch ? 6 : -1;

		if(!onlySearch)
		{
			embDB::FilePagePtr pPage = storage.getNewPage();
			storage.initStorage(pPage->getAddr());
			storage.saveStorageInfo();

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
			storage.setStoragePageInfo(0);
			storage.loadStorageInfo();

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo1.data", &storage, 1);
			tran.begin();
			TestPointSpatialSearchByQuery<TSparialTree,TCoodType, TTran, TZorderType>(50, nBegin, nEnd,  nStep,
				  qXmin,  qYmin,  qXmax,   qYmax,	&tran, alloc, nTreeRootPage);

		}
	}
}
void TestPointSpatialTree()
{
	/*uint64 nBegin = 0;
	uint64 nEnd = 1000;
	uint64 nStep = 1;
	uint64 xMin = 0;
	uint64 xMax = 100;
	uint64 yMin = 0;
	uint64 yMax = 100;*/

//	TestPointSpatial<TBPMapPOint16, uint16, embDB::CDirectTransactions, embDB::ZOrderPoint2DU16>("d:\\db\\dbspatialpoint16.data", 50, 8192, 0, 10000, 1, 0, 0, 100, 100, false);
//	TestPointSpatial<TBPMapPOint16, uint16, embDB::CDirectTransactions, embDB::ZOrderPoint2DU16>("d:\\db\\dbspatialpoint16.data", 50, 8192, 0, 10000, 1, 100, 0, 1000, 1000, true);


//	TestPointSpatial<TBPMapPOint32, uint32, embDB::CDirectTransactions, embDB::ZOrderPoint2DU32>("d:\\db\\dbspatialpoint32.data", 50, 8192, 0, 2000, 1, 0, 0, 100, 100, false);
//	TestPointSpatial<TBPMapPOint32, uint32, embDB::CDirectTransactions, embDB::ZOrderPoint2DU32>("d:\\db\\dbspatialpoint32.data", 50, 8192, 0, 10000, 1, 100, 0, 1000, 1000, true);

//	TestPointSpatial<TBPMapPOint64, uint64, embDB::CDirectTransactions, embDB::ZOrderPoint2DU64>("d:\\db\\dbspatialpoint64.data", 50, 8192, 0xFFFFFFFFFFFFFFFF-1000, 0xFFFFFFFFFFFFFFFF, 1, 0, 0, 100, 100, false);
    TestPointSpatial<TBPMapPOint64, uint64, embDB::CDirectTransaction, embDB::ZOrderPoint2DU64>("d:\\db\\dbspatialpoint64.data", 50, 8192, 0xFFFFFFFFFFFFFFFF-1000, 0xFFFFFFFFFFFFFFFF, 1, 0xFFFFFFFFFFFFFFFF-1000, 0xFFFFFFFFFFFFFFFF-1000, 0xFFFFFFFFFFFFFFFF-500, 0xFFFFFFFFFFFFFFFF-500, true);
}