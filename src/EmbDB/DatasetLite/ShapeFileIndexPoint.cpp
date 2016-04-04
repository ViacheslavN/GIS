#include "stdafx.h"
#include "ShapeCursor.h"
#include "SpatialTree.h"
#include "ShapeFileIndexPoint.h"

#include "../EmbDB/PointSpatialBPMapTree.h"
#include "CommonLibrary/SpatialKey.h"
#include "../EmbDB/SpatialPointQuery.h"
#include "../EmbDB/Transactions.h"
#include "../EmbDB/DirectTransactions.h"
#include "../EmbDB/RectSpatialBPMapTree.h"
#include "../EmbDB/SpatialRectQuery.h"
#include "../EmbDB/storage.h"
#include "../EmbDB/BaseInnerNodeDIffCompress.h"
#include "../EmbDB/BaseLeafNodeCompDiff.h"
#include "../EmbDB/PointZOrderCompressor.h"
#include "../EmbDB/RectZOrderCompressor.h"
#include "../EmbDB/TBaseSpatialCompressor.h"
#include "../EmbDB/UnsignedIntegerNumLenComp.h"

typedef embDB::TBaseSpatialCompress<embDB::ZOrderPoint2DU16, embDB::TPointZOrderCompressor<embDB::ZOrderPoint2DU16, uint16, 16> > TPointSpatialCompress16;
typedef embDB::TBaseSpatialCompress<embDB::ZOrderPoint2DU32, embDB::TPointZOrderCompressor<embDB::ZOrderPoint2DU32, uint32, 32> > TPointSpatialCompress32;
typedef embDB::TBaseSpatialCompress<embDB::ZOrderPoint2DU64, embDB::TPointZOrderCompressor<embDB::ZOrderPoint2DU64, uint64, 64> > TPointSpatialCompress64;

typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderPoint2DU16, TPointSpatialCompress16 > TPoint16InnerCompress;
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderPoint2DU16, uint32, embDB::IDBTransaction, TPointSpatialCompress16  , embDB::TUnsignedNumLenNodeCompressor<uint32, 32> > TPoint16LeafCompress;


typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderPoint2DU32, TPointSpatialCompress32 > TPoint32InnerCompress;
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderPoint2DU32, uint32, embDB::IDBTransaction, TPointSpatialCompress32  , embDB::TUnsignedNumLenNodeCompressor<uint32, 32> > TPoint32LeafCompress;


typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderPoint2DU64, TPointSpatialCompress64 > TPoint64InnerCompress;
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderPoint2DU64, uint32, embDB::IDBTransaction, TPointSpatialCompress64  , embDB::TUnsignedNumLenNodeCompressor<uint32, 32> > TPoint64LeafCompress;

typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU16, uint32,	embDB::ZPointComp<embDB::ZOrderPoint2DU16>, embDB::IDBTransaction, TPoint16InnerCompress, TPoint16LeafCompress> TBPMapPoint16;	
typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU32, uint32,	embDB::ZPointComp<embDB::ZOrderPoint2DU32>, embDB::IDBTransaction, TPoint32InnerCompress, TPoint32LeafCompress> TBPMapPoint32;
typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU64, uint32,	embDB::ZPointComp64, embDB::IDBTransaction, TPoint64InnerCompress, TPoint64LeafCompress> TBPMapPoint64;

namespace DatasetLite
{

	template<class TCoord, class TSPTree, int TreeType>
	class TStatialTreePoint : public IStatialTree
	{
	public:

		typedef typename TSPTree::TSpatialIterator TIterator;
		typedef typename TIterator::TPointKey TPointKey;
		typedef TShapeCursorPoint<TIterator, TPointKey>  TShapeCursor;


		TStatialTreePoint(CommonLib::alloc_t* pAlloc, embDB::CStorage* pStorage, int64 nTreeRootPageID, 
			double dOffsetX, double dOffsetY, double dScaleX, double dScaleY):
		m_Storage(pStorage), m_DBTran(pAlloc, pStorage), m_dOffsetX(dOffsetX), m_dOffsetY(dOffsetY),
			m_dScaleX(dScaleX), m_dScaleY(dScaleY), m_nTreeRootPageID(nTreeRootPageID)
		{

			m_SpatialTree.reset( new TSPTree(nTreeRootPageID, &m_DBTran, pAlloc, 50, 8192));
		}
		~TStatialTreePoint()
		{

		}
		virtual bool commit()
		{
			if(!m_SpatialTree.get())
				return false;

			return m_SpatialTree->commit();
		}
		virtual bool init(int64 nRootPage)
		{
			if(!m_SpatialTree.get())
				return false;
			m_nTreeRootPageID = nRootPage;
			m_SpatialTree->init(m_nTreeRootPageID); 
			return m_SpatialTree->commit();
		}
		virtual embDB::eDataTypes GetType()
		{
			return (embDB::eDataTypes)TreeType;
		}
		bool insert(double dX, double dY, int nShapeId)
		{
			TCoord xT = TCoord((dX + m_dOffsetX) / m_dScaleX);
			TCoord yT = TCoord((dY + m_dOffsetY) / m_dScaleY);

			return m_SpatialTree->insert(xT, yT, nShapeId);
		}
		bool insert(const CommonLib::bbox& extent, int nShapeId)
		{
			return insert(extent.xMin, extent.yMin, nShapeId);
		}

		virtual bool insert(ShapeLib::SHPObject* pObject, int nRow = -1 )
		{
			return insert(pObject->dfXMin, pObject->dfYMin, nRow == -1 ?  pObject->nShapeId : nRow);
		}
		IShapeCursorPtr search(const CommonLib::bbox& extent)
		{


			TCoord xMin = TCoord((extent.xMin + m_dOffsetX) / m_dScaleX);
			TCoord yMin = TCoord((extent.yMin + m_dOffsetY) / m_dScaleY);
			TCoord xMax = TCoord((extent.xMax + m_dOffsetX) / m_dScaleX);
			TCoord yMax = TCoord((extent.yMax + m_dOffsetY) / m_dScaleY);

			TIterator SPIterator = m_SpatialTree->spatialQuery(xMin, yMin, xMax, yMax);
			TShapeCursor *pCursor = new TShapeCursor(SPIterator, 
				m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY);
			return IShapeCursorPtr(pCursor);
		}
	private:
		std::auto_ptr<TSPTree> m_SpatialTree;
		embDB::CStorage* m_Storage;
		embDB::CDirectTransaction m_DBTran;
		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
		int64 m_nTreeRootPageID;


	};
	typedef TStatialTreePoint<uint16, TBPMapPoint16, embDB::stPoint16> TPointSpatialTreeU16;
	typedef TStatialTreePoint<uint32, TBPMapPoint32, embDB::stPoint32> TPointSpatialTreeU32;
	typedef TStatialTreePoint<uint64, TBPMapPoint64, embDB::stPoint64> TPointSpatialTreeU64;

	CShapeFileIndexPoint::CShapeFileIndexPoint(CommonLib::alloc_t* pAlloc, uint32 nPageSize, const CommonLib::bbox& bbox, double dOffsetX, double dOffsetY, double dScaleX, 
		double dScaleY, GisEngine::GisCommon::Units units, embDB::eSpatialType type, int nShapeType ) : 
	TBase(pAlloc, nPageSize, bbox, dOffsetX, dOffsetY, dScaleX, dScaleY, units, type, nShapeType)
	{

	}
	CShapeFileIndexPoint::CShapeFileIndexPoint(CommonLib::alloc_t* pAlloc) : TBase(pAlloc)
	{

	}
	CShapeFileIndexPoint::~CShapeFileIndexPoint()
	{

	}
	bool CShapeFileIndexPoint::Open(const CommonLib::CString& sDbName)
	{
		bool bBaseOpen =  TBase::Open(sDbName);
		if(!bBaseOpen)
			return false;

		switch(m_Type)
		{
		case embDB::stPoint16:
			m_SpTree.reset(new TPointSpatialTreeU16(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
				m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		case embDB::stPoint32:
			m_SpTree.reset(new TPointSpatialTreeU32(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
				m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		case embDB::stPoint64:
			m_SpTree.reset(new TPointSpatialTreeU64(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
				m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		}

		return bBaseOpen;
	}
	bool CShapeFileIndexPoint::Create(const CommonLib::CString& sDbName)
	{


		bool bBaseCreate =  TBase::Create(sDbName);
		switch(m_Type)
		{
		case embDB::stPoint16:
			m_SpTree.reset(new TPointSpatialTreeU16(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		case embDB::stPoint32:
			m_SpTree.reset(new TPointSpatialTreeU32(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		case embDB::stPoint64:
			m_SpTree.reset(new TPointSpatialTreeU64(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		}
		if(m_SpTree.get() == NULL)
			return false;

		m_SpTree->init(m_nRootTreePage);
		m_SpTree->commit();

		return true;
	}
	bool CShapeFileIndexPoint::commit()
	{
		if(m_SpTree.get())
			return m_SpTree->commit();
		return false;
	}
	bool CShapeFileIndexPoint::insert(const double dX, double dY, int nShapeId)
	{
		if(m_SpTree.get())
		{
			return m_SpTree->insert(dX, dY, nShapeId);
		}
		return false;
	}
}