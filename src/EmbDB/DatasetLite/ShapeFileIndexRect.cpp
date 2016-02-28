#include "stdafx.h"
#include "ShapeCursor.h"
#include "SpatialTree.h"
#include "ShapeFileIndexRect.h"

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
#include "../EmbDB/RectZOrderCompressor.h"
#include "../EmbDB/TBaseSpatialCompressor.h"



typedef embDB::TBaseSpatialCompress<embDB::ZOrderRect2DU16, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU16, uint16, 16> > TRectSpatialCompress16;
typedef embDB::TBaseSpatialCompress<embDB::ZOrderRect2DU32, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU32, uint32, 32> > TRectSpatialCompress32;
typedef embDB::TBaseSpatialCompress<embDB::ZOrderRect2DU64, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU64, uint64, 64> > TRectSpatialCompress64;


typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderRect2DU16, TRectSpatialCompress16 > TRect16InnerCompress;
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderRect2DU16, uint32, embDB::IDBTransaction, TRectSpatialCompress16 > TRect16LeafCompress;


typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderRect2DU32, TRectSpatialCompress32 > TRect32InnerCompress; 
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderRect2DU32, uint32, embDB::IDBTransaction, TRectSpatialCompress32> TRect32LeafCompress;

typedef embDB::TBPBaseInnerNodeDiffCompressor<embDB::ZOrderRect2DU64, TRectSpatialCompress64 > TRect64InnerCompress; 
typedef embDB::TBaseLeafNodeDiffComp<embDB::ZOrderRect2DU64, uint32, embDB::IDBTransaction, TRectSpatialCompress64 > TRect64LeafCompress;


typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU16, uint32, embDB::ZPointComp<embDB::ZOrderRect2DU16>, embDB::IDBTransaction, TRect16InnerCompress, TRect16LeafCompress> TBPMapRect16;
typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU32, uint32, embDB::ZRect32Comp, embDB::IDBTransaction, TRect32InnerCompress, TRect32LeafCompress > TBPMapRect32;
typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU64, uint32, embDB::ZRect64Comp, embDB::IDBTransaction, TRect64InnerCompress, TRect64LeafCompress> TBPMapRect64;
 

namespace DatasetLite
{

	template<class TCoord, class TSPTree, int TreeType>
	class TStatialTreeRect : public IStatialTree
	{
	public:

		typedef typename TSPTree::TSpatialIterator TIterator;
		typedef typename TIterator::TPointKey TPointKey;
		typedef TShapeCursorRect<TIterator, TPointKey>  TShapeCursor;

	
		TStatialTreeRect(CommonLib::alloc_t* pAlloc, embDB::CStorage* pStorage, int64 nTreeRootPageID, 
			double dOffsetX, double dOffsetY, double dScaleX, double dScaleY):
		m_Storage(pStorage), m_DBTran(pAlloc, pStorage), m_dOffsetX(dOffsetX), m_dOffsetY(dOffsetY),
			m_dScaleX(dScaleX), m_dScaleY(dScaleY), m_nTreeRootPageID(nTreeRootPageID)
		{

			m_SpatialTree.reset( new TSPTree(nTreeRootPageID, &m_DBTran, pAlloc, 50, 8192));
		}
		~TStatialTreeRect()
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
		bool insert(double xMin, double yMin, double xMax, double yMax, int nShapeId)
		{
			TCoord xMinT = TCoord((xMin + m_dOffsetX) / m_dScaleX);
			TCoord yMinT = TCoord((yMin + m_dOffsetY) / m_dScaleY);
			TCoord xMaxT = TCoord((xMax + m_dOffsetX) / m_dScaleX);
			TCoord yMaxT = TCoord((yMax + m_dOffsetY) / m_dScaleY);

			return m_SpatialTree->insert(xMinT, yMinT, xMaxT, yMaxT, nShapeId);
		}
		bool insert(const CommonLib::bbox& extent, int nShapeId)
		{
			return insert(extent.xMin, extent.yMin, extent.xMax, extent.yMax, nShapeId);
		}

		virtual bool insert(ShapeLib::SHPObject* pObject, int nRow = -1 )
		{
			return insert(pObject->dfXMin, pObject->dfYMin, pObject->dfXMax, pObject->dfYMax, nRow == -1 ?  pObject->nShapeId : nRow);
		}
		virtual bool insert(double dX, double dY, int nRow )
		{
			return insert(dX, dY, dX, dY, nRow);
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
	typedef TStatialTreeRect<uint16, TBPMapRect32, embDB::stRect16> TSpatialTreeU16;
	typedef TStatialTreeRect<uint32, TBPMapRect32, embDB::stRect32> TSpatialTreeU32;
	typedef TStatialTreeRect<uint64, TBPMapRect64, embDB::stRect64> TSpatialTreeU64;

	CShapeFileIndexRect::CShapeFileIndexRect(CommonLib::alloc_t* pAlloc, uint32 nPageSize, const CommonLib::bbox& bbox, double dOffsetX, double dOffsetY, double dScaleX, 
		double dScaleY, GisEngine::GisCommon::Units units, embDB::eSpatialType type, int nShapeType ) : 
		TBase(pAlloc, nPageSize, bbox, dOffsetX, dOffsetY, dScaleX, dScaleY, units, type, nShapeType)
	{

	}
	CShapeFileIndexRect::CShapeFileIndexRect(CommonLib::alloc_t* pAlloc) : TBase(pAlloc)
	{

	}
	CShapeFileIndexRect::~CShapeFileIndexRect()
	{

	}
	bool CShapeFileIndexRect::Open(const CommonLib::CString& sDbName)
	{
		bool bBaseOpen =  TBase::Open(sDbName);
		if(!bBaseOpen)
			return false;

		switch(m_Type)
		{
			case embDB::stRect16:
				m_SpTree.reset(new TSpatialTreeU16(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
					m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
					break;
			case embDB::stRect32:
				m_SpTree.reset(new TSpatialTreeU32(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
					m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
				break;
			case embDB::stRect64:
				m_SpTree.reset(new TSpatialTreeU64(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
					m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
				break;
		}

		return bBaseOpen;
	}
	bool CShapeFileIndexRect::Create(const CommonLib::CString& sDbName)
	{


		bool bBaseCreate =  TBase::Create(sDbName);
		switch(m_Type)
		{
		case embDB::stRect16:
			m_SpTree.reset(new TSpatialTreeU16(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		case embDB::stRect32:
			m_SpTree.reset(new TSpatialTreeU32(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		case embDB::stRect64:
			m_SpTree.reset(new TSpatialTreeU64(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		}
		if(m_SpTree.get() == NULL)
			return false;

		m_SpTree->init(m_nRootTreePage);
		m_SpTree->commit();
			
		return true;
	}
	bool CShapeFileIndexRect::commit()
	{
		if(m_SpTree.get())
			return m_SpTree->commit();
		return false;
	}
	bool CShapeFileIndexRect::insert(const CommonLib::bbox& extent, int nShapeId)
	{
		if(m_SpTree.get())
		{
			return m_SpTree->insert(extent, nShapeId);
		}
		return false;
	}
}