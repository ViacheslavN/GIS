#include "stdafx.h"
#include "ShapeCursor.h"
#include "SpatialTree.h"
#include "ShapeFileIndexRect.h"

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

	template<class TCoord, class TSPTree, int TreeType>
	class TStatialTreeRect : public IStatialTree
	{
	public:

		typedef typename TSPTree::TSpatialIterator TIterator;
		typedef typename TIterator::TPointKey TPointKey;
		typedef TShapeCursor<TIterator, TPointKey>  TShapeCursor;

	
		TStatialTreeRect(CommonLib::alloc_t* pAlloc, embDB::CStorage* pStorage, int64 nTreeRootPageID, 
			double dOffsetX, double dOffsetY, double dScaleX, double dScaleY):
		m_Storage(pStorage), m_DBTran(pAlloc, pStorage), m_dOffsetX(dOffsetX), m_dOffsetY(dOffsetY),
			m_dScaleX(dScaleX), m_dScaleY(dScaleY)
		{

			m_SpatialTree.reset( new TSPTree(nTreeRootPageID, &m_DBTran, pAlloc, 50));
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
		virtual bool init()
		{
			if(!m_SpatialTree.get())
				return false;

			m_SpatialTree->saveBTreeInfo(); 
			return m_SpatialTree->commit();
		}
		virtual embDB::eDataTypes GetType()
		{
			return (embDB::eDataTypes)TreeType;
		}
		virtual bool insert(ShapeLib::SHPObject* pObject)
		{


			TCoord xMin = TCoord((pObject->dfXMin + m_dOffsetX) / m_dScaleX);
			TCoord yMin = TCoord((pObject->dfYMin + m_dOffsetY) / m_dScaleY);
			TCoord xMax = TCoord((pObject->dfXMax + m_dOffsetX) / m_dScaleX);
			TCoord yMax = TCoord((pObject->dfYMax + m_dOffsetY) / m_dScaleY);

			//embDB::ZOrderRect2DU32 zVal(xMin, yMin, xMax, yMax);

			return m_SpatialTree->insert(xMin, yMin, xMax, yMax, pObject->nShapeId);

		}
		IShapeCursorPtr search(const CommonLib::bbox& extent)
		{

			TCoord xMin = TCoord((extent.xMin + m_dOffsetX) / m_dScaleX);
			TCoord yMin = TCoord((extent.yMin + m_dOffsetY) / m_dScaleY);
			TCoord xMax = TCoord((extent.xMax + m_dOffsetX) / m_dScaleX);
			TCoord yMax = TCoord((extent.yMax + m_dOffsetY) / m_dScaleY);

			TShapeCursor *pCursor = new TShapeCursor(m_SpatialTree->spatialQuery(xMin, yMin, xMax, yMax), 
				m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY);
			return IShapeCursorPtr(pCursor);
		}
	private:
		std::auto_ptr<TSPTree> m_SpatialTree;
		embDB::CStorage* m_Storage;
		embDB::CDirectTransactions m_DBTran;
		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
	 

	};

	typedef TStatialTreeRect<uint32, TBPMapRect32, embDB::dtRect32> TSpatialTreeU32;
	typedef TStatialTreeRect<uint64, TBPMapRect64, embDB::dtRect64> TSpatialTreeU64;

	CShapeFileIndexRect::CShapeFileIndexRect(CommonLib::alloc_t* pAlloc) : 
		TBase(pAlloc)
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
			case embDB::dtRect32:
				m_SpTree.reset(new TSpatialTreeU32(m_pAlloc, m_pStorage.get(), m_nRootTreePage,
					m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
				break;
		}

		return bBaseOpen;
	}
	bool CShapeFileIndexRect::Create(const CommonLib::CString& sDbName, size_t nPageSize, const CommonLib::CString& sShapeFileName)
	{
		ShapeLib::SHPHandle shFile = ShapeLib::SHPOpen(sShapeFileName.cstr(), "rb");
		if(!shFile)
			return false;

		bool bBaseCreate =  TBase::Create(sDbName, nPageSize, sShapeFileName, shFile);
		switch(m_Type)
		{
		case embDB::dtRect32:
			m_SpTree.reset(new TSpatialTreeU32(m_pAlloc, m_pStorage.get(), m_nRootTreePage, m_dOffsetX, m_dOffsetY, m_dScaleX, m_dScaleY));
			break;
		}
		m_SpTree->init();
		for (int i = 0; i < m_ObjectCount; ++i)
		{
			ShapeLib::SHPObject*  pObject = ShapeLib::SHPReadObject(shFile, i);
			m_SpTree->insert(pObject);
		}
		m_SpTree->commit();

		ShapeLib::SHPClose(shFile);
		return bBaseCreate;
	}
}