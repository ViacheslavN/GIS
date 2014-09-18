#ifndef _EMBEDDED_DATABASE_BP_MAP_RECT_SPATIAL_H_
#define _EMBEDDED_DATABASE_BP_MAP_RECT_SPATIAL_H_
#include "SpatialKey.h"
#include "PointSpatialBPMapTree.h"
#include "RectMapLeafCompressor.h"
#include "RectMapInnerCompressor.h"
namespace embDB
{
	template<class _TCoord, class _TValue, class _TComp, 
	class _Transaction = IDBTransactions,
	class _TInnerCompess =  BPSpatialPointInnerNodeSimpleCompressor<_TCoord> ,	
	class _TLeafCompess = BPSpatialPointLeafNodeMapSimpleCompressor<_TCoord, _TValue>
	>
    class TBPRectSpatialMap : public TBPPointSpatialMap<_TCoord, _TValue,  _TComp, _Transaction,
		_TInnerCompess, _TLeafCompess>
	{
	public:
		typedef TBPPointSpatialMap<_TCoord, _TValue, _TComp, _Transaction, _TInnerCompess, _TLeafCompess > TBase;
		typedef TBPMapV2<_TCoord, _TValue, _TComp, _Transaction, _TInnerCompess, _TLeafCompess > TSubBase;

			TBPRectSpatialMap(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize):
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize)
			{}


		typedef _TCoord        TPointKey;
		typedef typename TPointKey::TPointType  TPointType;
		typedef _TValue      TValue;
		typedef _TComp      TComp;
		typedef _Transaction      Transaction;
		typedef TRect2D<TPointType>         TRect;
		typedef typename TBase::TBTreeNode  TBTreeNode;
		typedef typename TBase::TLeafNode  TLeafNode;
		typedef typename TBase::iterator iterator;
		typedef IRefCntPtr<TBTreeNode> TBTreeNodePtr;

		/*template <class _TCoord, class _TValue, class _TComp, 	class _TInnerCompess ,	class _TLeafCompess,
		class _Transaction,	class _TBTreeNode, class _TLeftMemset,  class _TLeftNode>*/
	
		typedef typename TBase::TSpatialIterator TSpatialIterator;
			

		iterator identify(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax)
		{
			//TRect rectQuery(xMin, yMin, xMax, yMax);
			//if(!m_Extent.isIntersection(rectQuery))
			//	return iterator(this, NULL, NULL);

			TPointKey key(xMin, yMin , xMax, yMax);
			return TBase::lower_bound(key);
		}

		TSpatialIterator spatialQuery(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax)
		{
			TRect rectQuery(xMin, yMin, xMax, yMax);
			if(!m_Extent.isIntersection(rectQuery))
				return TSpatialIterator((TBase*)this);

			TPointKey zKeyMin(m_Extent.m_minX, m_Extent.m_minY, xMin, yMin);
			TPointKey zKeyMax(xMax, yMax, TPointKey::coordMax, TPointKey::coordMax);
			TBase::iterator it = TBase::lower_bound(zKeyMin);
			return TSpatialIterator(this, it.m_pCurNode.get(), it.m_nIndex, zKeyMin, zKeyMax, rectQuery);


			/*
			TBPTSpatialPointIteratorMap(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex, 
				 TPointKey& zMin, TPointKey& zMax, TRect& QueryRect) : 
			
			*/
		}

		bool insert(const TRect& rect, const TValue& val)
		{
			return insert(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY, val);
		}
		bool insert(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax, const TValue& val)
		{
			TRect rect(xMin, yMin, xMax, yMax);
			if(!rect.isInRect(m_Extent))
				return false;


			TPointKey key(xMin /*+ m_shiftX*/, yMin /*+ m_shiftY*/, xMax /*+ m_shiftX*/, yMax/* + m_shiftY*/);
			return TSubBase::insert(key, val); 
		}


	};

}


#endif