#ifndef _EMBEDDED_DATABASE_BP_MAP_RECT_SPATIAL_H_
#define _EMBEDDED_DATABASE_BP_MAP_RECT_SPATIAL_H_
#include "SpatialKey.h"
#include "PointSpatialBPMapTree.h"
#include "RectMapLeafCompressor.h"
#include "RectMapInnerCompressor.h"
namespace embDB
{
	template<class _TCoord, class _TValue, class _TComp, 
	class _TInnerCompess =  BPSpatialPointInnerNodeSimpleCompressor<RBMap<_TCoord, int64, _TComp> > ,	
	class _TLeafCompess = BPSpatialPointLeafNodeMapSimpleCompressor<RBMap<_TCoord, _TValue, _TComp> >,
	class _Transaction = IDBTransactions>
    class TBPRectSpatialMap : public TBPPointSpatialMap<_TCoord, _TValue,  _TComp, 
		_TInnerCompess, _TLeafCompess,	_Transaction>
	{
	public:
		typedef TBPPointSpatialMap<_TCoord, _TValue, _TComp, _TInnerCompess, _TLeafCompess,	_Transaction > TBase;
		typedef TBPMap<_TCoord, _TValue, _TComp, _TInnerCompess, _TLeafCompess,	_Transaction > TSubBase;

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
		typedef typename TBase::TLeafMemSet   TLeafMemSet;
		typedef typename TBase::TLeftMemSetNode  TLeftMemSetNode;
		typedef typename TBase::iterator iterator;

		/*template <class _TCoord, class _TValue, class _TComp, 	class _TInnerCompess ,	class _TLeafCompess,
		class _Transaction,	class _TBTreeNode, class _TLeftMemset,  class _TLeftNode>*/
	
		typedef typename TBase::TSpatialIterator TSpatialIterator;
			

		iterator identify(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax)
		{
			//TRect rectQuery(xMin, yMin, xMax, yMax);
			//if(!m_Extent.isIntersection(rectQuery))
			//	return iterator(this, NULL, NULL);

			TPointKey key(xMin/* - m_shiftX*/, yMin /*- m_shiftY*/, xMax/*- m_shiftX*/, yMax /*- m_shiftY*/);
			TBTreeNode* pFindBTNode = 0;
			TLeftMemSetNode *pFindRBNode= 0;
			SpatialfindNode(key,  &pFindBTNode, &pFindRBNode);
			return TBase::iterator(this, pFindBTNode, pFindRBNode );
		}

		TSpatialIterator spatialQuery(TPointType xMin, TPointType yMin, TPointType xMax, TPointType yMax)
		{
			TRect rectQuery(xMin, yMin, xMax, yMax);
			if(!m_Extent.isIntersection(rectQuery))
				return TSpatialIterator(this);

			TPointKey zKeyMin(m_Extent.m_minX, m_Extent.m_minY, xMin, yMin);
			TPointKey zKeyMax(xMax, yMax, TPointKey::coordMax, TPointKey::coordMax);
			TBTreeNode* pFindBTNode = 0;
			TLeftMemSetNode *pFindRBNode= 0;
			SpatialfindNode(zKeyMin,  &pFindBTNode, &pFindRBNode);
			return TSpatialIterator(this, pFindBTNode, pFindRBNode, zKeyMin, zKeyMax, rectQuery);
		}


		bool SpatialfindNode(const TKey& key, TBTreeNode** pFindBTNode,	TLeftMemSetNode **pFindMemNode)
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
				TLeftMemSetNode *pRBNode = m_pRoot->findLessOrEQNode(key);
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