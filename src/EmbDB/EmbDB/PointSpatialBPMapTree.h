#ifndef _EMBEDDED_DATABASE_BP_MAP_POINT_SPATIAL_H_
#define _EMBEDDED_DATABASE_BP_MAP_POINT_SPATIAL_H_
#include "BaseBPMapv2.h"
 
#include "PointSpatialBPMaTraits.h"
#include "SpatialKey.h"
#include "BPMapPointSpatialIterator.h"
namespace embDB
{
	template<class _TCoord, class _TValue, class _TComp, 
	class _Transaction = IDBTransactions,
	class _TInnerCompess =  BPSpatialPointInnerNodeSimpleCompressor<_TCoord > ,	
	class _TLeafCompess = BPSpatialPointLeafNodeMapSimpleCompressor<_TCoord>
	class _TInnerNode = BPTreeInnerNodeSetv2<_TCoord, _TComp, _Transaction, _TInnerCompess>,
	class _TLeafNode =  BPTreeLeafNodeMapv2<_TCoord, _TValue,  _TComp, _Transaction, _TLeafCompess>, 
	class _TBTreeNode = BPTreeNodeMapv2<_TCoord, _TValue,  _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
    class TBPPointSpatialMap : public TBPMapV2<_TCoord, _TValue,  _TComp, _Transaction
		_TInnerCompess, _TLeafCompess>
	{
	public:
		typedef TBPMapV2<_TCoord, _TValue, _TComp, _TInnerCompess, _TLeafCompess,	_Transaction > TBase;

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
		typedef typename TBase::TBTreeNode  TBTreeNode;
		typedef typename TBase::TLeafNode  TLeafNode;
		typedef typename TBase::iterator iterator;
		typedef TPoint2D<TPointType> TPoint;

		/*template <class _TCoord, class _TValue, class _TComp, 	class _TInnerCompess ,	class _TLeafCompess,
		class _Transaction,	class _TBTreeNode, class _TLeftMemset,  class _TLeftNode>*/
	
		typedef TBPTSpatialPointIteratorMap<_TCoord, _TValue, _TComp, _TInnerCompess, _TLeafCompess,	_Transaction, TBTreeNode, TLeafMemSet, TLeafNode
		> TSpatialIterator;
		
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
			TBTreeNode* pFindBTNode = 0;
			TLeftMemSetNode *pFindRBNode= 0;
			//SpatialfindNode(key,  &pFindBTNode, &pFindRBNode);
			return TBase::lower_bound(key);
			//return TBase::iterator(this, pFindBTNode, pFindRBNode );
		}

		TSpatialIterator spatialQuery(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax)
		{
			TRect rectQuery(xMin, yMin, xMax, yMax);
			if(!m_Extent.isIntersection(rectQuery))
				return TSpatialIterator(this);

			TPointKey zKeyMin(xMin /*- m_shiftX*/, yMin /*- m_shiftY*/);
			TPointKey zKeyMax(xMax /*- m_shiftX*/, yMax /*- m_shiftY*/);
			TBTreeNode* pFindBTNode = 0;
			TLeftMemSetNode *pFindRBNode= 0;
			TBase::iterator it = TBase::lower_bound(zKeyMin);
			//SpatialfindNode(zKeyMin,  &pFindBTNode, &pFindRBNode);
			return TSpatialIterator(this, it.m_pCurNode, it.m_nIndex, zKeyMin, zKeyMax, rectQuery);
		}


		/*bool SpatialfindNode(const TKey& key, TBTreeNode** pFindBTNode,	TLeftMemSetNode **pFindMemNode)
		{

			*pFindBTNode = NULL;
			*pFindMemNode= NULL;
			if(m_nRootAddr == -1)
				loadBTreeInfo();
			if(m_nRootAddr == -1)
				return false;

			if(!m_pRoot)
			{
				m_pRoot= getNode(m_nRootAddr, true); 
			}
			if(!m_pRoot)
				return false;

			if(m_pRoot-> isLeaf())
			{
				TLeftMemSetNode *pRBNode = m_pRoot->findNode(key);
				if(pRBNode)
				{
					*pFindBTNode = m_pRoot;
					*pFindMemNode = pRBNode;
				}

			}
			else
			{

				TInnerMemSetNode* pParentMemsetNode = 0;
				TBTreeNode* pInnerParentNode  = 0;
				pInnerParentNode = m_pRoot;
				int64 nNextAddr = m_pRoot->findNext(key, &pParentMemsetNode);
				for (;;)
				{
					if( nNextAddr == -1)
						break;
					TBTreeNode* pNode = getNode(nNextAddr, false, false, true);
					//pNode->m_nParent = pInnerParentNode->addr();
					//pNode->m_pParrentMemsetNode = pParentMemsetNode;
					if(pNode->isLeaf())
					{
						TLeftMemSetNode *pRBNode = pNode->findLessOrEQNode(key);
						if(!pRBNode)
						{
							pNode = getNode(pNode->next());
							if(pNode)
							{
								pRBNode = pNode->findLessOrEQNode(key);
							}
						}
						if(pRBNode)
						{	

							*pFindBTNode = pNode;
							*pFindMemNode = pRBNode;
							(*pFindBTNode)->m_bValidNextKey = false;

								TLeftMemSetNode *pPrev = pRBNode->m_pPrev;
								if(pNode->isLeamMemsetNodeNull(pPrev) && pNode->prev() != -1)
								{
									//TBTreeNode *pNode1 = pNode;
									TBTreeNode* pNodePrev = getNode(pNode->prev(), false, false, true);
									TLeftMemSetNode *pRBNode = pNodePrev->findLessOrEQNode(key);
									if(pRBNode)
									{

											if(pNodePrev->addr() == pInnerParentNode->less())
											{
												pNodePrev->m_bValidNextKey = true;
												pNodePrev->m_NextLeafKey = pInnerParentNode->firstInnerMemSetNode()->m_key;
											}
											else if(pParentMemsetNode)
											{
												if(!pInnerParentNode->isInnerMemsetNodeNull(pParentMemsetNode->m_pPrev)
													&& pParentMemsetNode->m_pPrev->m_val == pNodePrev->addr() )
												{
													pNodePrev->m_bValidNextKey = true;
													pNodePrev->m_NextLeafKey = pParentMemsetNode->m_key;
												}
												else
													pNodePrev->m_bValidNextKey = false;
											}
											else
												pNodePrev->m_bValidNextKey = false;


											*pFindBTNode = pNodePrev;
											*pFindMemNode = pRBNode;
										}
										
							}
							if(pNode->addr() == pInnerParentNode->less())
							{
									pNode->m_bValidNextKey = true;
									pNode->m_NextLeafKey = pInnerParentNode->firstInnerMemSetNode()->m_key;
							}
							else if(pParentMemsetNode)
							{
								if(!pInnerParentNode->isInnerMemsetNodeNull(pParentMemsetNode->m_pNext))
								{
									pNode->m_bValidNextKey = true;
									pNode->m_NextLeafKey = pParentMemsetNode->m_pNext->m_key;
								}
								else
									pNode->m_bValidNextKey = false;
							}
							else
								pNode->m_bValidNextKey = false;
						}
						break; //дальше искать не куда
					}
					pInnerParentNode = pNode;
					nNextAddr = pNode->findNext(key, &pParentMemsetNode);
				}
			}
			if(*pFindBTNode)
				(*pFindBTNode)->setFlags(BUSY_NODE, true);
			ClearChache();

			return true;
		}*/

		//TPointType getShiftX(){return m_shiftX; }
		//TPointType getShiftY(){return m_shiftY; }

	protected:
		TRect m_Extent;
		//TPointType m_shiftX;
		//TPointType m_shiftY;
	};

}


#endif