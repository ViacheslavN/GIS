#ifndef _EMBEDDED_DATABASE_BP_MAP_RECT_SPATIAL_H_
#define _EMBEDDED_DATABASE_BP_MAP_RECT_SPATIAL_H_
#include "CommonLibrary/SpatialKey.h"
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
		typedef CommonLib::TRect2D<TPointType>         TRect;
		typedef typename TBase::TBTreeNode  TBTreeNode;
		typedef typename TBase::TLeafNode  TLeafNode;
		typedef typename TBase::iterator iterator;
		typedef CommonLib::IRefCntPtr<TBTreeNode> TBTreeNodePtr;

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

		TSpatialIterator spatialQuery(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax, SpatialQueryMode mode = sqmIntersect)
		{
			TRect rectQuery(xMin, yMin, xMax, yMax);
			if(!m_Extent.isIntersection(rectQuery))
				return TSpatialIterator((TBase*)this);

			//TPointKey zKeyMin(m_Extent.m_minX, m_Extent.m_minY, xMin, yMin);
			//TPointKey zKeyMax(xMax, yMax, TPointKey::coordMax, TPointKey::coordMax);
			TPointKey zKeyMin;
			TPointKey zKeyMax;
			if(mode == sqmIntersect || mode == sqmByFeature)
			{
				zKeyMin.setZOrder(m_Extent.m_minX, m_Extent.m_minY, xMin, yMin);
				zKeyMax.setZOrder(xMax, yMax, TPointKey::coordMax, TPointKey::coordMax);
			}
			else if(mode = sqmOver)
			{
				zKeyMin.setZOrder(m_Extent.m_minX, m_Extent.m_minY, TPointKey::coordMax, TPointKey::coordMax);
				zKeyMax.setZOrder(xMin, yMin, xMax, yMax);
			}	
			else if(mode = sqmInside)
			{
				zKeyMin.setZOrder( xMin, yMin, xMax, yMax);
				zKeyMax.setZOrder(TPointKey::coordMax,TPointKey::coordMax,  m_Extent.m_minX, m_Extent.m_minY);
			}



			TBase::iterator it = TBase::lower_bound(zKeyMin);
			return TSpatialIterator(this, it.m_pCurNode.get(), it.m_nIndex, zKeyMin, zKeyMax, rectQuery);


			/*
			TBPTSpatialPointIteratorMap(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex, 
				 TPointKey& zMin, TPointKey& zMax, TRect& QueryRect) : 
			
			*/
		}

		bool insert(const TRect& rect, const TValue& val, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			return insert(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY, val, pFromIterator, pRetItertor);
		}
		bool insert(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax, const TValue& val, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			TRect rect(xMin, yMin, xMax, yMax);
			if(!rect.isInRect(m_Extent))
				return false;


			TPointKey key(xMin /*+ m_shiftX*/, yMin /*+ m_shiftY*/, xMax /*+ m_shiftX*/, yMax/* + m_shiftY*/);
			return TSubBase::insert(key, val,  pFromIterator, pRetItertor ); 
		}


	};

}


#endif