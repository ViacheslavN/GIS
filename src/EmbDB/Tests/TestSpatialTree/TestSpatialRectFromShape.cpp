#pragma once

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

#include "../../EmbDB/BaseInnerNodeDIffCompress.h"
#include "../../EmbDB/BaseLeafNodeCompDiff.h"
#include "../../EmbDB/PointZOrderCompressor.h"
#include "../../EmbDB/RectZOrderCompressor.h"
#include "../../EmbDB/TBaseSpatialCompressor.h" 


#include "ShapeLib/shapefil.h"
#include "CommonLibrary/File.h"
#include "CommonLibrary/BoundaryBox.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "../../EmbDB/SpatialRectQuery.h"



typedef embDB::TBaseSpatialCompress<embDB::ZOrderRect2DU16, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU16, uint16, 16> > TRectSpatialCompress16;
typedef embDB::TBaseSpatialCompress<embDB::ZOrderRect2DU32, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU32, uint32, 32> > TRectSpatialCompress32;
typedef embDB::TBaseSpatialCompress<embDB::ZOrderRect2DU64, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU64, uint64, 64> > TRectSpatialCompress64;


typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderRect2DU16, TRectSpatialCompress16 > TRect16InnerCompress;
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderRect2DU16, uint64, embDB::IDBTransaction, TRectSpatialCompress16 > TRect16LeafCompress;


typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderRect2DU32, TRectSpatialCompress32 > TRect32InnerCompress; 
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderRect2DU32, uint64, embDB::IDBTransaction, TRectSpatialCompress32> TRect32LeafCompress;

typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderRect2DU64, TRectSpatialCompress64 > TRect64InnerCompress; 
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderRect2DU64, uint64, embDB::IDBTransaction, TRectSpatialCompress64 > TRect64LeafCompress;


typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU16, uint64,
	embDB::ZPointComp<embDB::ZOrderRect2DU16> , embDB::IDBTransaction, TRect16InnerCompress, TRect16LeafCompress
> TBPMapRect16;

typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU32, uint64,
	embDB::ZRect32Comp, embDB::IDBTransaction,
	TRect32InnerCompress,	
	TRect32LeafCompress> TBPMapRect32;




typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU64, uint64,
	embDB::ZRect64Comp, embDB::IDBTransaction,
	TRect64InnerCompress,	
	TRect64LeafCompress > TBPMapRect64;




template<class TSparialTree, class TTran, class TZorderType, class TUnits>
int  ReadShape(const wchar_t* pszShapeFileName, TTran* pTran, CommonLib::alloc_t *pAlloc, uint32 nCacheBPTreeSize, uint32 nPageNodeSize, int64& nTreeRootPage, CommonLib::TRect2D<TUnits>& ExtentTree)
{

	TSparialTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, nPageNodeSize);
	tree.loadBTreeInfo(); 
	//

	embDB::CBPTreeStatistics statInfo;
	tree.SetBPTreeStatistics(&statInfo);



	SHPGuard shp;
	DBFGuard dbf;

	CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(pszShapeFileName);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(pszShapeFileName);
	CommonLib::CString shpFilePath = sFilePath + sFileName + L".shp";
	CommonLib::CString dbfFilePath = sFilePath + sFileName + L".dbf";
	CommonLib::CString prjFileName = sFilePath + sFileName + L".prj";


	shp.file = ShapeLib::SHPOpen(shpFilePath.cstr(), "rb");
	if(!shp.file)
		return;
	dbf.file = ShapeLib::DBFOpen(dbfFilePath.cstr(), "rb");
	if(!dbf.file)
		return; 




	int objectCount;
	int shapeType;
	double minBounds[4];
	double maxBounds[4];
	ShapeLib::SHPGetInfo(shp.file, &objectCount, &shapeType, &minBounds[0], &maxBounds[0]);
	CommonLib::bbox bounds;

	if(objectCount > 0)
	{
		bounds.type = CommonLib::bbox_type_normal;
		bounds.xMin = minBounds[0];
		bounds.xMax = maxBounds[0];
		bounds.yMin = minBounds[1];
		bounds.yMax = maxBounds[1];
		bounds.zMin = minBounds[2];
		bounds.zMax = maxBounds[2];
		bounds.mMin = minBounds[3];
		bounds.mMax = maxBounds[3];
	}
	double dOffsetX, dOffsetY;
	if(bounds.xMin < 0)
		dOffsetX = fabs(bounds.xMin);
	else
		dOffsetX = -1 *bounds.xMin;

	if(bounds.yMin < 0)
		dOffsetY = fabs(bounds.yMin);
	else
		dOffsetY = -1 *bounds.yMin;


	



	ExtentTree.m_minX = TUnits((bb.xMin + dOffsetX) / dScale);
	ExtentTree.m_minY = TUnits((bb.yMin + dOffsetY) / dScale);
	ExtentTree.m_maxX = TUnits((bb.xMax + dOffsetX) / dScale);
	ExtentTree.m_maxY = TUnits((bb.yMax + dOffsetY) / dScale);


	tree.setExtent(ExtentTree);


	double dScale = 0.0000001;
	CommonLib::CGeoShape shape;
	shape.AddRef();
	ShapeLib::SHPObject*   pCacheObject = NULL;
	for(size_t row = 0; row < objectCount; ++row)
	{
		pCacheObject = ShapeLib::SHPReadObject(shp.file, row);
		SHPObjectToGeometry(pCacheObject, shape);


		//CommonLib::MemoryStream steram;
		// shape.write(&steram);
		//nShapeRowSize += steram.pos();
		if(pCacheObject)
		{
			ShapeLib::SHPDestroyObject(pCacheObject);
			pCacheObject = 0;
		}

		CommonLib::bbox bb = shape.getBB();

		TUnits xMin = TUnits((bb.xMin + dOffsetX) / dScale);
		TUnits yMin = TUnits((bb.yMin + dOffsetY) / dScale);
		TUnits xMax = TUnits((bb.xMax + dOffsetX) / dScale);
		TUnits yMax = TUnits((bb.yMax + dOffsetY) / dScale);

		tree.insert(xMin, yMin, xMax, yMax, row);
	 
	}

	tree.commit();
	pTran->commit();

	return objectCount;
}


template <class TSparialTree, class TTran, class TZorderType, class TUnits>
void TestRectSpatialSplitSearch(const CommonLib::TRect2D<TUnits>& ExtentTree, const CommonLib::TRect2D<TUnits>& SpatialQuery,
		int nCacheStorageSize, int nPageSize, int64 nTreeRootPage, std::set<int64>& pOIDs)
{
	TSparialTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, nPageNodeSize);
	tree.loadBTreeInfo(); 
	tree.setExtent(ExtentTree);

	TSparialTree::TSpatialIterator it = tree.spatialQuery(SpatialQuery.m_minX, SpatialQuery.m_minY, SpatialQuery.m_maxX, SpatialQuery.m_maxY);

	while(!it.isNull())
	{

		pOIDs.insert(it.value());
		it.next();
	}

}



template <class TSparialTree, class TTran, class TZorderType, class TUnits>
void TestRectSpatialFullSearch(const CommonLib::TRect2D<TUnits>& ExtentTree, const CommonLib::TRect2D<TUnits>& SpatialQuery,
	int nCacheStorageSize, int nPageSize, int64 nTreeRootPage, std::set<int64>& pOIDs)
{
	TSparialTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, nPageNodeSize);
	tree.loadBTreeInfo(); 
	tree.setExtent(ExtentTree);


	TZorderType zKeyMin;
	TZorderType zKeyMax;

	zKeyMin.setZOrder(ExtentTree.m_minX, ExtentTree.m_minY, SpatialQuery.m_minX, SpatialQuery.m_minY);
	zKeyMax.setZOrder(SpatialQuery.m_maxX, SpatialQuery.m_maxY, TZorderType::coordMax, TZorderType::coordMax);

	TSparialTree::iterator it = tree.lower_bound(zKeyMin);

	while(!it.isNull())
	{

		TZorderType zKey = it.key(); 
		if(zKey < it.key())
			break;

		if(zKey.IsInRect(SpatialQuery))
			pOIDs.insert(it.value());
		it.next();
	}

}



template <class TSparialTree, class TTran, class TZorderType, class TUnits>
void TestRectSpatialImp(const CommonLib::CString& sFileName,  const CommonLib::CString& sShapeFileName,  int nCacheStorageSize, int nPageSize)
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	{
		embDB::CStorage storage( alloc, nCacheStorageSize);
		storage.open(sFileName.cwstr(), false, false,  true, false);



		int64 nTreeRootPage = -1;
		int64 nStorageInfoPage = -1;
		int nCount =  0;
		 CommonLib::TRect2D<TUnits> extent;

		  CommonLib::TRect2D<TUnits> rectQuery;;

		 {
			embDB::FilePagePtr pPage = storage.getNewPage(nPageSize);
			nStorageInfoPage = pPage->getAddr();
			storage.initStorage(nStorageInfoPage);
			storage.saveStorageInfo();
			pPage.release();

			{
				TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\db\\createtran.data", &storage, 1);
				tran.begin();
				embDB::FilePagePtr pPage = tran.getNewPage(256);

				nTreeRootPage = pPage->getAddr();
				pPage.release();
				TSparialTree tree(-1, &tran, alloc, 10, 8192);
				tree.init(nTreeRootPage); 
				tran.commit();
			}

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo.data", &storage, 1);
			tran.begin();
			nCount = ReadShape<TSparialTree,TZorderType, TTran, TUnits>(sShapeFileName.cwstr(), &tran, alloc, 50, nPageSize, nTreeRootPage, extent);
		 }


		 std::set<__int64> setSplitOID, setFullSearchID;
	/*	if(nType == SEARCH)
		{
			storage.setStoragePageInfo(0);
			storage.loadStorageInfo();
			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranSearch.data", &storage, 1);
			tran.begin();
			TestRectSpatialSearchByQuery<TSparialTree,TCoodType, TTran, TZorderType>(50, nBegin, nEnd, nStep,  qXmin,   qYmin,   qXmax,   qYmax,   &tran, alloc, nPageSize, nTreeRootPage);

		}
		if(nType == IDENTIFY)
		{

			TTran tran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranSearch.data", &storage, 1);
			tran.begin();
			TestRectSpatialSearchByFeature<TSparialTree,TCoodType, TTran, TZorderType>(50, nBegin, nEnd, nStep, &tran, alloc, nPageSize, nTreeRootPage);

		}*/
	}
}


void TestRectSpatialTreeFromShape()
{

}