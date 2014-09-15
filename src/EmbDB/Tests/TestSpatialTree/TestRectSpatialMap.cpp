#include "stdafx.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include "../../EmbDB/RectSpatialBPMapTree.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/DebugTime.h"
#include <iostream>
#include <set>

#define  INSERT 1
#define  SEARCH 2
#define  IDENTIFY 3

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
	if(!nProcStep)
		nProcStep = 1000000;

	embDB::TRect2D<TCoodType>         feature;
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
void TestRectSpatialSearchByQuery(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, TCoodType nSearchStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	embDB::TRect2D<TCoodType>         ExtentTree;
	embDB::TRect2D<TCoodType>         SpatialQuery;
	embDB::TRect2D<TCoodType>         rectFeature;
	embDB::TRect2D<TCoodType>         ZrectFeature;

	std::vector<embDB::TRect2D<TCoodType>  > vecRrectFeature;
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
	TCoodType xMin, yMin, xMax, yMax;
	nSearchStep = 1;
	std::set<uint64> m_Zal;
	std::set<uint64> m_OID;
	int64 nOrderError = 0;
	TZorderType zFullVal;
 
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
		SpatialQuery.m_minY = (TCoodType)nStart;
		SpatialQuery.m_maxY = (TCoodType)nEndStart;
	
		//for (uint64 y = nStart; y <= nEndStart - nSearchStep; y +=nSearchStep)
		
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

					TZorderType zLast;
					int ff = 0;
					while(!it.isNull())
					{
						++nCnt;

						zVal = it.key();
					
						int64 nOID = it.value();
						//	if(zVal > zValMax)
						//		break;
						if(nCnt == 345)
						{
							int dd = 0;
							dd++;
						}
						if(m_ODIs.find(nOID) != m_ODIs.end())
						{
							nDouble++;
						}
						else
						{
							m_ODIs.insert(nOID);
						}
									
						if(zLast > zVal)
						{
							nOrderError++;
						}
						zLast = zVal;
										
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
							vecRrectFeature.push_back(ZrectFeature);
							++nCntInQuery;
					//		m_Zal.erase(zVal.m_nZValue);
					//		m_OID.erase(it.value());
						}
						if(nCntInQuery == 168)
						{
							int ndd = 0;
							ndd++;
						}
					

						if(!it.next())
							break;
					}


				}
					searchZorderTm = time.stop();

			}
			

	
		{
			
			SpatialQuery.m_minY = (TCoodType)nStart;
			SpatialQuery.m_maxY = (TCoodType)nEndStart;
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
			m_ODIsFull.clear();
			TSparialTree::iterator it = tree.identify(0, 0, SpatialQuery.m_minX, SpatialQuery.m_minY);
			time.start();

			while(!it.isNull())
			{
			
			break;
				zFullVal = it.key();

				int64 nOID = it.value();
				if(zFullVal > zValMax)
					break;
				/*if(zFullVal.m_nZValue == 1229782938247337104)
				{
					int dd = 0;
					dd++;
				}*/
				
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
			//		m_Zal.insert(zFullVal.m_nZValue);
					m_Zal.insert(it.value());
				}

				if(nOID == 78648)
				{
					int dd = 0;
					dd++;
				}

				if(nOID == 78649)
				{
					int dd = 0;
					dd++;
				}
				if(!it.next())
					break;
			}
		}
			searchFullTm = time.stop();

		
		
		

			std::cout << "Found Zorder  time: " << searchZorderTm <<" Cnt: " << nCnt << " InQuery " <<  nCntInQuery<< " NotInQuery " << nCntNotQuery << " Order Error " << nOrderError  <<" Double " << nDouble << std::endl;
			std::cout << "Found Full    time: " << searchFullTm   <<" Cnt: " << nCntFull << " InQuery " <<  nCntInQueryFull<< " NotInQuery " << nCntNotQueryFull  << std::endl;

			if(m_Zal.size())	
			{
				std::set<uint64>::iterator it = m_Zal.begin();
				std::set<uint64>::iterator end = m_Zal.end();

				for(;it != end; ++it)
				{
					if(m_ODIs.find((*it)) == m_ODIs.end())
						std::cout << "Not Found " << (*it) << std::endl;
				}
			}
			
			
			return;
		}

	


	searchTm = time.stop();
	std::cout << "Search end key start: " << nStart << " key end: " << nEndStart << " Not found: " << nNotFound<< " OID Error found: " << nOIDErr  << " Total time: " << searchTm << std::endl;

}


template <class TSparialTree, class TCoodType, class Tran, class TZorderType>
void TestRectSpatialSearchByFeature(int32 nCacheBPTreeSize, uint64 nStart, uint64 nEndStart, uint64 nStep, Tran* pTran, CommonLib::alloc_t *pAlloc, int64 nTreeRootPage)
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
void TestRectSpatial(const CommonLib::str_t& sFileName,  int nCacheStorageSize, int nPageSize, TCoodType nBegin, TCoodType nEnd, TCoodType nStep, TCoodType nSearchStep, int nType)
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::TimeUtils::CDebugTime time;
	time.start();
	std::cout <<"Begin test Count: Begin: " << nBegin << " End: " << nEnd << " Step: " << nStep << " PageSize :" << nPageSize << std::endl;
	{
		embDB::CStorage storage( alloc, nCacheStorageSize);
		storage.open(sFileName, false, false,  nType != INSERT ? false : true, false, nPageSize);
		embDB::CFilePage* pPage = storage.getNewPage();
		storage.setStoragePageInfo(pPage->getAddr());
		storage.saveStorageInfo();

		int64 nTreeRootPage = nType != INSERT ? 3 : -1;

		if(nType == INSERT)
		{
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo.data", &storage, 1);
			tran.begin();
			TestRectSpatialInsert<TSparialTree,TCoodType, TTran>(50, nBegin, nEnd, nStep, &tran, alloc, nTreeRootPage);
			return;
		}
		if(nType == SEARCH)
		{

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranSearch.data", &storage, 1);
			tran.begin();
			TestRectSpatialSearchByQuery<TSparialTree,TCoodType, TTran, TZorderType>(50, nBegin, nEnd, nStep, nSearchStep,  &tran, alloc, nTreeRootPage);
			
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
	 //TestRectSpatial<TBPMapRect16, uint16, embDB::CDirectTransactions, embDB::ZOrderRect2DU16>("d:\\dbspatialrect16.data", 50, 8192, 0, 0xFFFF, 10, 0, INSERT);
	 TestRectSpatial<TBPMapRect16, uint16, embDB::CDirectTransactions, embDB::ZOrderRect2DU16>("d:\\dbspatialrect16.data", 50, 8192, 0, 1000, 10, 10, SEARCH);
	// TestRectSpatial<TBPMapRect16, uint16, embDB::CDirectTransactions, embDB::ZOrderRect2DU16>("d:\\dbspatialrect16.data", 50, 8192, 0, 0xFFFF,100, 1000, IDENTIFY);

	//TestRectSpatial<TBPMapRect32Ugd, uint32, embDB::CDirectTransactions, ugdRectKey>("d:\\dbspatialrect32Ugd.data", 50, 8192, 0, 0xFFFF, 10, 0, INSERT);
    //TestRectSpatial<TBPMapRect32Ugd, uint32, embDB::CDirectTransactions, ugdRectKey>("d:\\dbspatialrect32Ugd.data", 50, 8192, 0, 160, 10, 10, SEARCH);

	


	// TestRectSpatial<TBPMapRect32, uint32, embDB::CDirectTransactions, embDB::ZOrderRect2DU32>("d:\\dbspatialrect32.data", 50, 8192,  0 , 0xFFFF, 10, 0, INSERT);
	// TestRectSpatial<TBPMapRect32, uint32, embDB::CDirectTransactions, embDB::ZOrderRect2DU32>("d:\\dbspatialrect32.data", 50, 8192,0, 160,10, 10, SEARCH);
	// TestRectSpatial<TBPMapRect32, uint32, embDB::CDirectTransactions, embDB::ZOrderRect2DU32>("d:\\dbspatialrect32.data", 50, 8192, 0, 0xFFFF,10, 10, IDENTIFY);
	
	//TestRectSpatial<TBPMapRect64, uint64, embDB::CDirectTransactions, embDB::ZOrderRect2DU64>("d:\\dbspatialrect64.data", 50, 8192,   0xFFFFFFFFFFFFFFFF - 60000, 0xFFFFFFFFFFFFFFFF, 10, 0, INSERT);
	//TestRectSpatial<TBPMapRect64, uint64, embDB::CDirectTransactions, embDB::ZOrderRect2DU64>("d:\\dbspatialrect64.data", 50, 8192, 0xFFFFFFFFFFFFFFFF - 60000, 0xFFFFFFFFFFFFFFFF - 50000,10, 10, SEARCH);
	//TestRectSpatial<TBPMapRect64, uint64, embDB::CDirectTransactions, embDB::ZOrderRect2DU64>("d:\\dbspatialrect64.data", 50, 8192, 0, 0xFFFF,10, 10, IDENTIFY);
}