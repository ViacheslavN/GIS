#include "stdafx.h"
#include "ShapeFileIndex.h"
#include "../EmbDB/PointSpatialBPMapTree.h"
#include "CommonLibrary/SpatialKey.h"
#include "../EmbDB/SpatialPointQuery.h"
#include "../EmbDB/Transactions.h"
#include "../EmbDB/DirectTransactions.h"
#include "../EmbDB/PoinMapInnerCompressor64.h"
#include "../EmbDB/PoinMapLeafCompressor64.h"
#include "../EmbDB/RectSpatialBPMapTree.h"
#include "../EmbDB/SpatialRectQuery.h"
#include "../EmbDB/storage.h"
#include "PointIndex.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "ShapeFileIndexRect.h"

typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU16, uint32,
	embDB::ZPointComp<embDB::ZOrderPoint2DU16>, embDB::CStorage > TBPMapPoint16;


typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU32, uint32,
	embDB::ZPointComp<embDB::ZOrderPoint2DU32>, embDB::CStorage  > TBPMapPoint32;


typedef embDB::TBPPointSpatialMap<
	embDB::ZOrderPoint2DU64, 	uint32,	embDB::ZPointComp64, 
	embDB::CStorage,
	embDB::BPSpatialPointInnerNodeSimpleCompressor64,
	embDB::BPSpatialPointLeafNodeMapSimpleCompressor64<uint32> 
> TBPMapPoint64;







namespace DatasetLite
{

	typedef TPointIndex<TBPMapPoint16, uint16, embDB::CStorage> TPointIndex16;
	typedef TPointIndex<TBPMapPoint32, uint32, embDB::CStorage> TPointIndex32;
	typedef TPointIndex<TBPMapPoint64, uint64, embDB::CStorage> TPointIndex64;

	

	IShapeFileIndexPtr IShapeFileIndex::open(const CommonLib::CString& sDbName)
	{ 
		CShapeFileIndexRect *pShapeFileIndexRect = new CShapeFileIndexRect();

		pShapeFileIndexRect->Open(sDbName);
		return IShapeFileIndexPtr(pShapeFileIndexRect);
	}


	IShapeFileIndexPtr IShapeFileIndex::create(const CommonLib::CString& sDbName, size_t nPageSize, const CommonLib::CString& sShapeFileName)
	{ 
		CShapeFileIndexRect *pShapeFileIndexRect = new CShapeFileIndexRect();

		pShapeFileIndexRect->Create(sDbName, nPageSize, sShapeFileName);
		return IShapeFileIndexPtr(pShapeFileIndexRect);
	}

}
