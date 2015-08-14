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


typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU16, uint64,
	embDB::ZPointComp<embDB::ZOrderRect2DU16> 
> TBPMapRect16;

typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU32, uint64,
	embDB::ZRect32Comp, embDB::IDBTransactions,
embDB::BPSpatialRectInnerNodeSimpleCompressor<embDB::ZOrderRect2DU32 >,	
embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU32, uint64> > TBPMapRect32;



 
typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU64, uint64,
	embDB::ZRect64Comp, embDB::IDBTransactions,
	embDB::BPSpatialRectInnerNodeSimpleCompressor<embDB::ZOrderRect2DU64>,	
	embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU64, uint64 > > TBPMapRect64;





namespace DatasetLite
{
	typedef TPointIndex<TBPMapPoint16, uint16, embDB::CStorage> TPointIndex16;
	typedef TPointIndex<TBPMapPoint32, uint32, embDB::CStorage> TPointIndex32;
	typedef TPointIndex<TBPMapPoint64, uint64, embDB::CStorage> TPointIndex64;


}
