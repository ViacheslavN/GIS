#include "stdafx.h"
#include "ShapeCursor.h"
#include "SpatialTree.h"
#include "ShapeFileIndexPoint.h"

#include "../EmbDB/DB/BTreePlus/spatial/Point/PointSpatialBPMapTree.h"
#include "CommonLibrary/SpatialKey.h"
#include "../EmbDB/DB/BTreePlus/spatial/Point/SpatialPointQuery.h"
#include "../EmbDB/DB/transactions/Transactions.h"
#include "../EmbDB/DB/transactions/DirectTran/DirectTransactions.h"
#include "../EmbDB/DB/BTreePlus/spatial/Rect/RectSpatialBPMapTree.h"
#include "../EmbDB/DB/BTreePlus/spatial/Rect/SpatialRectQuery.h"
#include "../EmbDB/storage/storage.h"
 

#include "../EmbDB/DB/BTreePlus/BaseNodeCompressor.h"
#include "../EmbDB/Utils/compress/NumLen/SignedNumLenEncoder.h"
#include "../EmbDB/Utils/compress/NumLen/UnsignedNumLenEncoder.h"
#include "../EmbDB/DB/Indexs/spatial/ZOrderCompressBase.h"
#include "../EmbDB/DB/Fields//BaseFieldEncoders.h"


namespace DatasetLite
{

	typedef embDB::TValueDiffEncoder<int32, int32, embDB::SignedNumLenEncoder32> TIndexOIDEncoder;
	typedef embDB::TValueDiffEncoder<int64, int64, embDB::SignedNumLenEncoder64> TLinkEncoder;

	typedef embDB::TBasePointCompressor<embDB::ZOrderPoint2DU16, uint16, embDB::TPointZOrderCompressor<embDB::ZOrderPoint2DU16, uint16, 16> > TPointSpatialEncoder16;
	typedef embDB::TBasePointCompressor<embDB::ZOrderPoint2DU32, uint32, embDB::TPointZOrderCompressor<embDB::ZOrderPoint2DU32, uint32, 32> > TPointSpatialEncoder32;
	typedef embDB::TBasePointCompressor<embDB::ZOrderPoint2DU64, uint64, embDB::TPointZOrderCompressor<embDB::ZOrderPoint2DU64, uint64, 64> > TPointSpatialEncoder64;
	
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderPoint2DU16, int32, embDB::IDBTransaction, TPointSpatialEncoder16, TIndexOIDEncoder> TPointNodeCompressor16;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderPoint2DU32, int32, embDB::IDBTransaction, TPointSpatialEncoder32, TIndexOIDEncoder> TPointNodeCompressor32;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderPoint2DU64, int32, embDB::IDBTransaction, TPointSpatialEncoder64, TIndexOIDEncoder> TPointNodeCompressor64;


	typedef embDB::TBaseNodeCompressor<embDB::ZOrderPoint2DU16, int64, embDB::IDBTransaction, TPointSpatialEncoder16, TLinkEncoder> TPointInnerNodeCompressor16;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderPoint2DU32, int64, embDB::IDBTransaction, TPointSpatialEncoder32, TLinkEncoder> TPointInnerNodeCompressor32;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderPoint2DU64, int64, embDB::IDBTransaction, TPointSpatialEncoder64, TLinkEncoder> TPointInnerNodeCompressor64;

	typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU16, int32, embDB::ZPointComp<embDB::ZOrderPoint2DU16>, embDB::IDBTransaction, TPointInnerNodeCompressor16, TPointNodeCompressor16> TBPMapPoint16;
	typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU32, int32, embDB::ZPointComp<embDB::ZOrderPoint2DU32>, embDB::IDBTransaction, TPointInnerNodeCompressor32, TPointNodeCompressor32> TBPMapPoint32;
	typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU64, int32, embDB::ZPointComp64, embDB::IDBTransaction, TPointInnerNodeCompressor64, TPointNodeCompressor64> TBPMapPoint64;



	template<class TCoord, class TSPTree, int TreeType>
	class TStatialTreePoint : public IStatialTree
	{
	public:

		typedef typename TSPTree::TSpatialIterator TIterator;
		typedef typename TIterator::TPointKey TPointKey;
		typedef TShapeCursorPoint<TIterator, TPointKey>  TShapeCursor;


		TStatialTreePoint(CommonLib::alloc_t* pAlloc, embDB::CStorage* pStorage, int64 nTreeRootPageID, 
			double dOffsetX, double dOffsetY, byte nScaleX, byte nScaleY):
		m_Storage(pStorage), m_DBTran(pAlloc, embDB::rtUndefined, embDB::eTT_UNDEFINED, L"", pStorage), m_dOffsetX(dOffsetX), m_dOffsetY(dOffsetY),
			m_nScaleX(nScaleX), m_nScaleY(nScaleY), m_nTreeRootPageID(nTreeRootPageID)
 		{
			m_dCalcScaleX = 1/pow(10., m_nScaleX);
			m_dCalcScaleY = 1/pow(10., m_nScaleY);
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
			TCoord xT = TCoord((dX + m_dOffsetX) / m_dCalcScaleX);
			TCoord yT = TCoord((dY + m_dOffsetY) / m_dCalcScaleY);

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


			TCoord xMin = TCoord((extent.xMin + m_dOffsetX) / m_dCalcScaleX);
			TCoord yMin = TCoord((extent.yMin + m_dOffsetY) / m_dCalcScaleY);
			TCoord xMax = TCoord((extent.xMax + m_dOffsetX) / m_dCalcScaleX);
			TCoord yMax = TCoord((extent.yMax + m_dOffsetY) / m_dCalcScaleY);

			TIterator SPIterator = m_SpatialTree->spatialQuery(xMin, yMin, xMax, yMax);
			TShapeCursor *pCursor = new TShapeCursor(SPIterator, 
				m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY);
			return IShapeCursorPtr(pCursor);
		}
	private:
		std::auto_ptr<TSPTree> m_SpatialTree;
		embDB::CStorage* m_Storage;
		embDB::CDirectTransaction m_DBTran;
		double m_dOffsetX;
		double m_dOffsetY;
		byte m_nScaleX;
		byte m_nScaleY;
		double m_dCalcScaleX;
		double m_dCalcScaleY;
		int64 m_nTreeRootPageID;


	};
	typedef TStatialTreePoint<uint16, TBPMapPoint16, embDB::stPoint16> TPointSpatialTreeU16;
	typedef TStatialTreePoint<uint32, TBPMapPoint32, embDB::stPoint32> TPointSpatialTreeU32;
	typedef TStatialTreePoint<uint64, TBPMapPoint64, embDB::stPoint64> TPointSpatialTreeU64;

	CShapeFileIndexPoint::CShapeFileIndexPoint(CommonLib::alloc_t* pAlloc, uint32 nPageSize, const CommonLib::bbox& bbox, double dOffsetX, double dOffsetY, byte nScaleX, 
		byte nScaleY, GisEngine::GisCommon::Units units, embDB::eSpatialType type, int nShapeType ) : 
	TBase(pAlloc, nPageSize, bbox, dOffsetX, dOffsetY, nScaleX, nScaleY, units, type, nShapeType)
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
				m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
			break;
		case embDB::stPoint32:
			m_SpTree.reset(new TPointSpatialTreeU32(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
				m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
			break;
		case embDB::stPoint64:
			m_SpTree.reset(new TPointSpatialTreeU64(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
				m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
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
			m_SpTree.reset(new TPointSpatialTreeU16(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
			break;
		case embDB::stPoint32:
			m_SpTree.reset(new TPointSpatialTreeU32(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
			break;
		case embDB::stPoint64:
			m_SpTree.reset(new TPointSpatialTreeU64(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
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