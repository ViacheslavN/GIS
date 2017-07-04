#include "stdafx.h"
#include "ShapeCursor.h"
#include "SpatialTree.h"
#include "ShapeFileIndexRect.h"

#include "../EmbDB/DB/BTreePlus/spatial/Point/PointSpatialBPMapTree.h"
#include "CommonLibrary/SpatialKey.h"
#include "../EmbDB/DB/BTreePlus/spatial/Point/SpatialPointQuery.h"
#include "../EmbDB/DB/transactions/Transactions.h"
#include "../EmbDB/DB/transactions/DirectTran/DirectTransactions.h"
 
#include "../EmbDB/DB/BTreePlus/spatial/Rect/RectSpatialBPMapTree.h"
#include "../EmbDB/DB/BTreePlus/spatial/Rect/SpatialRectQuery.h"
#include "../EmbDB/Storage/storage.h"


#include "../EmbDB/DB/BTreePlus/BaseNodeCompressor.h"
#include "../EmbDB/Utils/compress/NumLen/SignedNumLenEncoder.h"
#include "../EmbDB/Utils/compress/NumLen/UnsignedNumLenEncoder.h"
#include "../EmbDB/DB/Indexs/spatial/ZOrderCompressBase.h"
#include "../EmbDB/DB/Fields//BaseFieldEncoders.h"
 
 
namespace DatasetLite
{

	typedef embDB::TValueDiffEncoder<int32, int32, embDB::SignedNumLenEncoder32> TIndexOIDEncoder;
	typedef embDB::TValueDiffEncoder<int64, int64, embDB::SignedNumLenEncoder64> TLinkEncoder;

	typedef embDB::TBaseRectCompressor<embDB::ZOrderRect2DU16, uint16, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU16, uint16, 16> > TRectSpatialEncoder16;
	typedef embDB::TBaseRectCompressor<embDB::ZOrderRect2DU32, uint32, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU32, uint32, 32> > TRectSpatialEncoder32;
	typedef embDB::TBaseRectCompressor<embDB::ZOrderRect2DU64, uint64, embDB::TRectZOrderCompressor<embDB::ZOrderRect2DU64, uint64, 64> > TRectSpatialEncoder64;


	typedef embDB::TBaseNodeCompressor<embDB::ZOrderRect2DU16, int32, embDB::IDBTransaction, TRectSpatialEncoder16, TIndexOIDEncoder> TRectNodeCompressor16;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderRect2DU32, int32, embDB::IDBTransaction, TRectSpatialEncoder32, TIndexOIDEncoder> TRectNodeCompressor32;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderRect2DU64, int32, embDB::IDBTransaction, TRectSpatialEncoder64, TIndexOIDEncoder> TRectNodeCompressor64;

	typedef embDB::TBaseNodeCompressor<embDB::ZOrderRect2DU16, int64, embDB::IDBTransaction, TRectSpatialEncoder16, TLinkEncoder> TRectInnerNodeCompressor16;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderRect2DU32, int64, embDB::IDBTransaction, TRectSpatialEncoder32, TLinkEncoder> TRectInnerNodeCompressor32;
	typedef embDB::TBaseNodeCompressor<embDB::ZOrderRect2DU64, int64, embDB::IDBTransaction, TRectSpatialEncoder64, TLinkEncoder> TRectInnerNodeCompressor64;
	

	typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU16, int32, embDB::ZPointComp<embDB::ZOrderRect2DU16>, embDB::IDBTransaction, TRectInnerNodeCompressor16, TRectNodeCompressor16> TBPMapRect16;
	typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU32, int32, embDB::ZRect32Comp, embDB::IDBTransaction, TRectInnerNodeCompressor32, TRectNodeCompressor32 > TBPMapRect32;
	typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU64, int32, embDB::ZRect64Comp, embDB::IDBTransaction, TRectInnerNodeCompressor64, TRectNodeCompressor64> TBPMapRect64;


	template<class TCoord, class TSPTree, int TreeType>
	class TStatialTreeRect : public IStatialTree
	{
	public:

		typedef typename TSPTree::TSpatialIterator TIterator;
		typedef typename TIterator::TPointKey TPointKey;
		typedef TShapeCursorRect<TIterator, TPointKey>  TShapeCursor;

	
		TStatialTreeRect(CommonLib::alloc_t* pAlloc, embDB::CStorage* pStorage, int64 nTreeRootPageID, 
			double dOffsetX, double dOffsetY, byte nScaleX, byte nScaleY):
		m_Storage(pStorage), m_DBTran(pAlloc, embDB::rtUndefined, embDB::eTT_UNDEFINED, L"", pStorage), m_dOffsetX(dOffsetX), m_dOffsetY(dOffsetY),
			m_nScaleX(nScaleX), m_nScaleY(nScaleY), m_nTreeRootPageID(nTreeRootPageID)
		{

			m_dCalcScaleX = 1/pow(10., m_nScaleX);
			m_dCalcScaleY = 1/pow(10., m_nScaleY);

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
			TCoord xMinT = TCoord((xMin + m_dOffsetX) / m_dCalcScaleX);
			TCoord yMinT = TCoord((yMin + m_dOffsetY) / m_dCalcScaleY);
			TCoord xMaxT = TCoord((xMax + m_dOffsetX) / m_dCalcScaleX);
			TCoord yMaxT = TCoord((yMax + m_dOffsetY) / m_dCalcScaleY);

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
	typedef TStatialTreeRect<uint16, TBPMapRect32, embDB::stRect16> TSpatialTreeU16;
	typedef TStatialTreeRect<uint32, TBPMapRect32, embDB::stRect32> TSpatialTreeU32;
	typedef TStatialTreeRect<uint64, TBPMapRect64, embDB::stRect64> TSpatialTreeU64;

	CShapeFileIndexRect::CShapeFileIndexRect(CommonLib::alloc_t* pAlloc, uint32 nPageSize, const CommonLib::bbox& bbox, double dOffsetX, double dOffsetY, byte nScaleX, 
		byte nScaleY, GisEngine::GisCommon::Units units, embDB::eSpatialType type, int nShapeType ) : 
		TBase(pAlloc, nPageSize, bbox, dOffsetX, dOffsetY, nScaleY, nScaleY, units, type, nShapeType)
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
					m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
					break;
			case embDB::stRect32:
				m_SpTree.reset(new TSpatialTreeU32(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
					m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
				break;
			case embDB::stRect64:
				m_SpTree.reset(new TSpatialTreeU64(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
					m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
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
			m_SpTree.reset(new TSpatialTreeU16(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
			break;
		case embDB::stRect32:
			m_SpTree.reset(new TSpatialTreeU32(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
			break;
		case embDB::stRect64:
			m_SpTree.reset(new TSpatialTreeU64(m_pAlloc, m_pStorage.get(), -1, m_dOffsetX, m_dOffsetY, m_nScaleX, m_nScaleY));
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