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


typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU16, uint32,
	embDB::ZPointComp<embDB::ZOrderRect2DU16> 
> TBPMapRect16;

typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU32, uint32,
	embDB::ZRect32Comp, embDB::IDBTransactions,
embDB::BPSpatialRectInnerNodeSimpleCompressor<embDB::ZOrderRect2DU32 >,	
embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU32, uint32> > TBPMapRect32;



 
typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU64, uint32,
	embDB::ZRect64Comp, embDB::IDBTransactions,
	embDB::BPSpatialRectInnerNodeSimpleCompressor<embDB::ZOrderRect2DU64>,	
	embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU64, uint32 > > TBPMapRect64;





namespace DatasetLite
{

	typedef TPointIndex<TBPMapPoint16, uint16, embDB::CStorage> TPointIndex16;
	typedef TPointIndex<TBPMapPoint32, uint32, embDB::CStorage> TPointIndex32;
	typedef TPointIndex<TBPMapPoint64, uint64, embDB::CStorage> TPointIndex64;



	class CStatialTreeRect32 : public IStatialTree
	{
		public:
			CStatialTreeRect32(CommonLib::alloc_t* pAlloc, embDB::CStorage* pStorage, int64 nTreeRootPageID):
				m_Storage(pStorage), m_DBTran(pAlloc, pStorage)
			{

				m_SpatialTree.reset( new TBPMapRect32(nTreeRootPageID, &m_DBTran, pAlloc, 50));
			}
			~CStatialTreeRect32()
			{

			}

			virtual embDB::eDataTypes GetType()
			{
				return embDB::dtRect32;
			}
			virtual bool insert(ShapeLib::SHPObject* pObject)
			{
			

				uint32 xMin = uint32((pObject->dfXMin + m_dOffsetX) / m_dScaleX);
				uint32 yMin = uint32((pObject->dfYMin + m_dOffsetY) / m_dScaleY);
				uint32 xMax = uint32((pObject->dfXMax + m_dOffsetX) / m_dScaleX);
				uint32 yMax = uint32((pObject->dfYMax + m_dOffsetY) / m_dScaleY);
				
				//embDB::ZOrderRect2DU32 zVal(xMin, yMin, xMax, yMax);

				return m_SpatialTree->insert(xMin, yMin, xMax, yMax, pObject->nShapeId);

			}
			IShapeCursorPtr search(CommonLib::bbox& extent)
			{
				return IShapeCursorPtr();
			}
	private:
		std::auto_ptr<TBPMapRect32> m_SpatialTree;
		embDB::CStorage* m_Storage;
		embDB::CDirectTransactions m_DBTran;
		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
	};


	CShapeFileIndexRect::CShapeFileIndexRect(const wchar_t *pszFileName)
	{

	}
	CShapeFileIndexRect::~CShapeFileIndexRect()
	{

	}
	const CommonLib::bbox& CShapeFileIndexRect::extent() const
	{
		return m_extent;
	}
	IShapeCursorPtr CShapeFileIndexRect::spatialQuery(const CommonLib::bbox& extent)
	{
		return IShapeCursorPtr();
	}



	IShapeFileIndexPtr IShapeFileIndex::create(const CommonLib::CString& sDbName, size_t nPageSize, const CommonLib::CString& sShapeFileName)
	{ 
		ShapeLib::SHPHandle shFile = ShapeLib::SHPOpen(sShapeFileName.cstr(), "rb");
		if(!shFile)
			return IShapeFileIndexPtr();

		CommonLib::simple_alloc_t alloc;
		embDB::CStorage mStorage(&alloc);

		if(!mStorage.open(sDbName.cwstr(), false, false,  true, false, nPageSize))
		{
			return IShapeFileIndexPtr();
		}

		CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(sShapeFileName);
		CommonLib::CString sFileName = CommonLib::FileSystem::FindFileName(sShapeFileName);
		CommonLib::CString prjFileName = sFilePath + sFileName + L".prj";

		int objectCount;
		int shapeType;
		double minBounds[4];
		double maxBounds[4];
		SHPGetInfo(shFile, &objectCount, &shapeType, &minBounds[0], &maxBounds[0]);
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

		GisEngine::GisGeometry::CSpatialReferenceProj4* pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(prjFileName, GisEngine::GisGeometry::eSPRefTypePRJFilePath);
		if(!pSpatialReference->IsValid())
		{
			pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(bounds);
		}


		GisEngine::GisCommon::Units units = GisEngine::GisCommon::UnitsUnknown;
		if(pSpatialReference->IsValid())
		{
			units = pSpatialReference->GetUnits();
		}

		 embDB::eDataTypes type =  embDB::dtShape64;

		double dOffsetX = 0., dOffsetY = 0., dScaleX = 0., dScaleY = 0.;

		if(bounds.xMin < 0)
			dOffsetX = fabs(bounds.xMin);
		if(bounds.yMin < 0)
			dOffsetY = fabs(bounds.yMin);

		if(units == GisEngine::GisCommon::UnitsDecimalDegrees)
		{
			dScaleX = 0.0000001;
			dScaleY = 0.0000001;
			type = embDB::dtShape32;
		}
		else if(units == GisEngine::GisCommon::UnitsMeters)
		{
			/*uint32 nInt32Max = 0xFFFFFFFF;
			int64 ndistX = fabs(bounds.xMax - bounds.xMin);
			int64 ndistY = fabs(bounds.yMax - bounds.yMin);
			*/
			dScaleX = 0.001;
			dScaleY = 0.001;
		}

		if(shapeType == SHPT_POINT || shapeType == SHPT_POINTZ || shapeType == SHPT_POINTM || 
			shapeType == SHPT_MULTIPOINT || shapeType == SHPT_MULTIPOINTZ || shapeType == SHPT_MULTIPOINTM)
		{
			if(type == embDB::dtShape32)
				type = embDB::dtPoint32;
			else
				type = embDB::dtPoint64;
		}
		else
		{
			if(type == embDB::dtShape32)
				type = embDB::dtRect32;
			else
				type = embDB::dtRect64;
		}
		 


		embDB::FilePagePtr pPage = mStorage.getFilePage(0, false);
		
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());	
		stream.write(bounds.xMin);
		stream.write(bounds.yMin);
		stream.write(bounds.xMax);
		stream.write(bounds.yMax);
		stream.write(shapeType);

		stream.write(uint32(units));
		stream.write(dOffsetX);
		stream.write(dOffsetY);
		stream.write(dScaleX);
		stream.write(dScaleY);
	
		stream.write((uint32)type);


		delete pSpatialReference;
		return IShapeFileIndexPtr();
	}

}
