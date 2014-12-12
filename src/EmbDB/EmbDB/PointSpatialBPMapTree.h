#ifndef _EMBEDDED_DATABASE_BP_MAP_POINT_SPATIAL_H_
#define _EMBEDDED_DATABASE_BP_MAP_POINT_SPATIAL_H_
#include "BaseBPMapv2.h"
 
//#include "PointSpatialBPMaTraits.h"
#include "SpatialKey.h"
#include "PoinMapInnerCompressor.h"
#include "PoinMapLeafCompressor.h"
#include "BPMapPointSpatialIterator.h"

namespace embDB
{
	template<class _TCoord, class _TValue, class _TComp, 
	class _Transaction = IDBTransactions,
	class _TInnerCompess =  BPSpatialPointInnerNodeSimpleCompressor<_TCoord > ,	
	class _TLeafCompess = BPSpatialPointLeafNodeMapSimpleCompressor<_TCoord, _TValue>,
	class _TInnerNode = BPTreeInnerNodeSetv2<_TCoord,/* _TComp,*/ _Transaction, _TInnerCompess>,
	class _TLeafNode =  BPTreeLeafNodeMapv2<_TCoord, _TValue, /* _TComp, */_Transaction, _TLeafCompess>, 
	class _TBTreeNode = BPTreeNodeMapv2<_TCoord, _TValue, /* _TComp,*/ _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
    class TBPPointSpatialMap : public TBPMapV2<_TCoord, _TValue,  _TComp, _Transaction,
		_TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
	{
	public:
		typedef TBPMapV2<_TCoord, _TValue, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode > TBase;

			TBPPointSpatialMap(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize):
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, true)/*,
				m_shiftX(0), m_shiftY(0)*/
			{}


		typedef _TCoord        TPointKey;
		typedef typename TPointKey::TPointType  TPointType;
		typedef _TValue      TValue;
		typedef _TComp      TComp;
		typedef _Transaction      Transaction;
		typedef TRect2D<TPointType>         TRect;
		typedef typename TBase::TInnerNode  TInnerNode;
		typedef typename TBase::TBTreeNode  TBTreeNode;
		typedef typename TBase::TLeafNode  TLeafNode;
		typedef typename TBase::iterator iterator;
		typedef TPoint2D<TPointType> TPoint;

	
		typedef TBPTSpatialPointIteratorMap<_TCoord, _TValue, _TComp, _Transaction, _TInnerCompess, _TLeafCompess,
			 TInnerNode, TLeafNode, TBTreeNode> TSpatialIterator;
		
		bool insert(const TPoint& point, const TValue& val)
		{
			return insert(point.m_x, point.m_y, val);
		}
		bool insert(TPointType x, TPointType y, const TValue& val)
		{
			if(x < m_Extent.m_minX || x > m_Extent.m_maxX)
				return false;
			if(y < m_Extent.m_minY || y > m_Extent.m_maxY)
				return false;

			TPointKey key(x/* + m_shiftX*/, y/* + m_shiftY*/);
			return TBase::insert(key, val); 
		}
		void setExtent(TRect& extent)
		{
			m_Extent = extent;

		//	m_shiftX = m_Extent.m_minX < 0 ? -1 * m_Extent.m_minX : 0;
		//	m_shiftY = m_Extent.m_minY < 0 ? -1 * m_Extent.m_minY : 0;
		}

		iterator identify(TPointType x, TPointType y)
		{

			if(x < m_Extent.m_minX || x > m_Extent.m_maxX)
				return iterator(this, NULL, NULL);
			if(y < m_Extent.m_minY || y > m_Extent.m_maxY)
				return iterator(this, NULL, NULL);
			TPointKey key(x /*- m_shiftX*/, y /*- m_shiftY*/);
			return TBase::lower_bound(key);
		}

		TSpatialIterator spatialQuery(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax)
		{
			TRect rectQuery(xMin, yMin, xMax, yMax);
			if(!m_Extent.isIntersection(rectQuery))
				return TSpatialIterator(this);

			TPointKey zKeyMin(xMin, yMin);
			TPointKey zKeyMax(xMax, yMax);
			TBase::iterator it = TBase::lower_bound(zKeyMin);
			return TSpatialIterator(this, it.m_pCurNode.get(), it.m_nIndex, zKeyMin, zKeyMax, rectQuery);
		}

	protected:
		TRect m_Extent;

	};

}


#endif