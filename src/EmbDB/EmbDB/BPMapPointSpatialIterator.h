#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_POINT_ITERATOR_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_POINT_ITERATOR_MAP_H_
//#include "PointSpatialBPMaTraits.h"
#include "BPTreeNodeMap.h"
#include "SpatialKey.h"
#include "simple_stack.h"
namespace embDB
{
	template<class _TCoord, class _TValue, class _TComp, 
	class _Transaction,	class _TInnerCompess ,	class _TLeafCompess,
	class _TInnerNode ,	class _TLeafNode, class _TBTreeNode>
	class TBPPointSpatialMap;


	template <class _TCoord, class _TValue, class _TComp, 		class _Transaction, class _TInnerCompess ,	class _TLeafCompess,
	class _TInnerNode, class _TLeafNode,  class _TBTreeNode>
	class TBPTSpatialPointIteratorMap
	{
	public:

		typedef  _TCoord     TPointKey;
		typedef typename TPointKey::TPointType  TPointType;
		typedef _TValue    TValue;
		typedef  _TComp	   TComp;
		typedef	 _Transaction  Transaction;
		typedef _TLeafNode TBTreeLeafNode;
		typedef  _TBTreeNode TBTreeNode;
		typedef TRect2D<TPointType>         TRect;
		/*


	
		*/

		typedef TBPPointSpatialMap<_TCoord, _TValue, _TComp, _Transaction,	_TInnerCompess ,_TLeafCompess,
		 _TInnerNode ,	_TLeafNode, _TBTreeNode> TBTree;


		struct TQuery
		{
			TPointKey m_zMin;
			TPointKey m_zMax;
			TRect minRect;
			TRect maxRect;
			short m_nBits;
			short m_ID;
	


			TQuery() : m_ID (0)
			{

			}

			void FromZ()
			{
				m_zMin.getXY(minRect.m_minX, minRect.m_minY, minRect.m_maxX, minRect.m_maxY);
				m_zMax.getXY(maxRect.m_minX, maxRect.m_minY, maxRect.m_maxX, maxRect.m_maxY);
			}
		};


		TBPTSpatialPointIteratorMap(TBTree *pTree) :
			m_pTree(pTree), m_pCurNode(NULL), m_pCurLeafNode(NULL)
		{}


		TBPTSpatialPointIteratorMap(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex, 
			TPointKey& zMin, TPointKey& zMax, TRect& QueryRect) : 
		m_pTree(pTree), m_zMin(zMin), m_zMax(zMax), m_QueryRect(QueryRect), m_nIndex(nIndex)
		{		
			m_nReq = 0;
			m_ID = 1;
			m_CurrentSpatialQuery.m_zMin = m_zMin;
			m_CurrentSpatialQuery.m_zMax = m_zMax;
			m_CurrentSpatialQuery.m_nBits = m_zMin.getBits();
			m_CurrentSpatialQuery.m_ID = m_ID;
			m_pCurNode = pCurNode;
			if(pCurNode)
			{
				SetParams(pCurNode);
				CreateSubQuery();
			}

			
		}

		void SetParams(TBTreeNode *pCurNode)
		{
		
			if(m_pCurNode && m_nIndex != -1)
			{
				assert(m_pCurNode->isLeaf());
				m_pCurNode->setFlags(BUSY_NODE, true);
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;

				TPointKey& zMinPage = m_pCurNode->key(m_nIndex);
				m_CurrentSpatialQuery.m_zMin = zMinPage;

			}
		}
		TBPTSpatialPointIteratorMap(const TBPTSpatialPointIteratorMap& iter)
		{

			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, true);

		}
		~TBPTSpatialPointIteratorMap()
		{
			if(m_pCurNode)
				m_pCurNode->setFlags(BUSY_NODE, false);
		}
		TBPTSpatialPointIteratorMap& operator = (const TBPTSpatialPointIteratorMap& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;

			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, true);
			return this;
		}

		const TPointKey& key() const
		{
			return m_pCurNode->key(m_nIndex);
		}
		const TValue& value()  const
		{
			return m_pCurNode->value(m_nIndex);
		}

		TPointKey& key()
		{
				return m_pCurNode->key(m_nIndex);
		}
		TValue& value()
		{
			return m_pCurNode->value(m_nIndex);
		}

		bool isNull()
		{
			if(m_pCurNode == NULL || m_nIndex == - 1)
				return true;
					
			TPointKey& zPage = m_pCurNode->key(m_nIndex);
			if(m_zMax < zPage)
				return true;
			return false;
		}

		bool findNext(bool bNextQuery = true)
		{
			while(true)
			{
				if(bNextQuery)
				{
					if(m_Queries.empty())
					{
						m_pCurNode = NULL;
						return false;
					}
					m_CurrentSpatialQuery = m_Queries.top();
				}
				bNextQuery = true;
				if(m_pCurNode)
					m_pCurNode->setFlags(BUSY_NODE, false);

				
				TBTree::iterator it = m_pTree->lower_bound(m_CurrentSpatialQuery.m_zMin);
		
				if(it.isNull())
				{
					if(!it.m_pCurNode)
						continue;
					TBTreeNode *pNode = m_pTree->getNode(m_pCurNode->next());
					if(!pNode)
						return false;
					short  nType;
					int nIndex = pNode->leaf_lower_bound(m_CurrentSpatialQuery.m_zMin, nType);
					if(nIndex != -1)
					{
						if(pNode->key(nIndex) > m_CurrentSpatialQuery.m_zMax)
							continue;

						m_nIndex = nIndex;
						m_pCurNode->setFlags(BUSY_NODE, false);
						m_pCurNode = pNode;
						m_pCurNode->setFlags(BUSY_NODE, true);
						SetParams(m_pCurNode);
						CreateSubQuery();
						return true;
					}
					continue;
				}
				TPointKey& zPage = it.key();
				if(it.key() > m_CurrentSpatialQuery.m_zMax)
					continue;
			
				m_nIndex = it.m_nIndex;
				m_pCurNode->setFlags(BUSY_NODE, false);
				m_pCurNode = it.m_pCurNode;
				m_pCurNode->setFlags(BUSY_NODE, true);
				SetParams(m_pCurNode);
				CreateSubQuery();
				return true;
			
			}
		}

		bool next()
		{
			if(!m_pCurNode)
				return false;
		
			m_pTree->ClearChache();
			m_nIndex++;
			if(m_nIndex < (int32)m_pCurLeafNode->count())
			{

				TPointKey& zPage = m_pCurLeafNode->key(m_nIndex);
				if(m_zMax < zPage && m_Queries.empty())
				{
					m_pCurNode = NULL;
					return false;
				}
				if( m_CurrentSpatialQuery.m_zMax < zPage)
				{
					if(m_Queries.empty())
					{
						m_pCurNode = NULL;
						return false;
					}
					m_CurrentSpatialQuery = m_Queries.top();
					if(m_CurrentSpatialQuery.m_zMin <= zPage)
					{
						m_CurrentSpatialQuery.m_zMin = zPage;
						CreateSubQuery();
						if(zPage > m_CurrentSpatialQuery.m_zMax)
						{
							m_nReq++;
							return findNext(false);
						}
						return true;
					}
					m_pCurNode->setFlags(BUSY_NODE, false);
					TBTree::iterator it = m_pTree->lower_bound(m_CurrentSpatialQuery.m_zMin);
					if(it.isNull())
					{
						return findNext(false);
					}
					else
					{
						if(it.key() > m_CurrentSpatialQuery.m_zMax)
						{
							m_nReq++;
							return findNext(false);
						}
				
						m_nIndex = it.m_nIndex;
						//if(m_pCurNode->addr() != it.m_pCurNode->addr())
						//{
							m_pCurNode->setFlags(BUSY_NODE, false);
							m_pCurNode = it.m_pCurNode;
							m_pCurNode->setFlags(BUSY_NODE, true);
							SetParams(m_pCurNode);
							CreateSubQuery();
						//}
						/*if(it.key() > m_CurrentSpatialQuery.m_zMax)
						{
							m_nReq++;
							return findNext(false);
						}*/
						return true;
					}
			
				}
		
			}
			else
			{
				TBTreeNode *pParentNode = m_pTree->getNode(m_pCurNode->m_nParent, false, false, true);
				int32 nParentNextKeyIndex = -1;
				if(!pParentNode)
				{
				
				}
				else if(pParentNode->less() == m_pCurNode->addr())
				{
					nParentNextKeyIndex = 0;
				}
				else
				{
					
					if((m_pCurNode->m_nFoundIndex + 1) < (int32)pParentNode->count())
						nParentNextKeyIndex = m_pCurNode->m_nFoundIndex + 1;
				}
				if(nParentNextKeyIndex != -1)
				{
					TPointKey& zLastPage = m_pCurNode->key(m_pCurNode->count() - 1);
					TPointKey& zFirtNextPage =pParentNode->key(nParentNextKeyIndex);
					if( m_pCurNode->key(m_pCurNode->count() - 1) == pParentNode->key(nParentNextKeyIndex))
					{
						m_pCurNode->setFlags(BUSY_NODE, false); 
						m_pCurNode = m_pTree->getNode(m_pCurNode->next(), false, false, true);
						m_pCurNode->setFlags(BUSY_NODE, true); 
						m_nIndex = 0;
						return true;
					}
					else
					{
						return findNext();
					}
				
				}
	
				TBTreeNode *pBPNode = m_pTree->getNode(m_pCurNode->next(), false, false, true);
				if(!pBPNode)
				{
					m_pCurNode->setFlags(BUSY_NODE, false); 
					m_nIndex = -1;
					m_pCurNode = NULL;
					return false;
						/*return findNext();*/
				}
				return findNext();
			}	


		
			return true;
		}
		void update()
		{
			assert(!isNull());
			m_pCurNode->setFlags(CHANGE_NODE, true);
		}

		

		bool bCheckBit(short nBit, uint64 zMin, uint64 zMax)
		{
			short nFirstBit = -1;
			for(short idx = 63; idx >= 0; --idx)
			{
				if((zMin >> (idx & 0x3f)) != (zMax >> (idx & 0x3f)))
				{
					nFirstBit = idx;
					break;
				}
			}

			return nBit >= nFirstBit;
		}
		void CreateSubQuery()
		{
			if(m_pCurNode->count() == 0)
			{
				assert(false);
				return;
			}
			//TPointKey zPageLast(m_currentNodePageRect.m_maxX, m_currentNodePageRect.m_maxY);
			/*(!pLastMemSet)
				 pLastMemSet = m_pCurNode->lastLeftMemSetNode();*/
			TPointKey zPageLast = m_pCurLeafNode->key(m_pCurLeafNode->count() - 1);
			//TPointKey& highKey = m_CurrentSpatialQuery.m_zMax;
			//TQuery curQuery = m_CurrentSpatialQuery;
			//TRect nRectLast;
			//zPageLast.getXY(nRectLast.m_minX, nRectLast.m_minY, nRectLast.m_maxX, nRectLast.m_maxY);
			short nBeginCurBit = m_CurrentSpatialQuery.m_nBits;
			TQuery BegincurQuery = m_CurrentSpatialQuery;

			if(BegincurQuery.m_ID == 45)
			{
				int dd = 0;
				dd++;
			}

			//std::vector<zInfo> vecQuery;
			while (zPageLast < m_CurrentSpatialQuery.m_zMax)
			{

				if(m_CurrentSpatialQuery.m_zMin == m_CurrentSpatialQuery.m_zMax)
					break;

				/*if(m_CurrentSpatialQuery.m_nBits < 0)
				{
					m_CurrentSpatialQuery.m_nBits = 63;
				}*/
				
				assert(m_CurrentSpatialQuery.m_nBits >= 0);
				while (m_CurrentSpatialQuery.m_zMin.getBit (m_CurrentSpatialQuery.m_nBits) == m_CurrentSpatialQuery.m_zMax.getBit (m_CurrentSpatialQuery.m_nBits))
				{
					
					m_CurrentSpatialQuery.m_nBits--;
					assert(m_CurrentSpatialQuery.m_nBits >= 0);
				}


				/*
				spatial4Query *subQuery=createQuery(q);
				subQuery->prevQuery = q.queryHead;
				subQuery->lowKey = q.queryHead->lowKey;
				subQuery->highKey = q.queryHead->highKey;
				subQuery->highKey.setLowBits (q.queryHead->curBitNum);
				q.queryHead->lowKey.clearLowBits (q.queryHead->curBitNum);
				subQuery->curBitNum = --q.queryHead->curBitNum;
				q.queryHead = subQuery;
				
				*/
				 

		
				// запрос с большими ключами
			//	nNexSubQuery.m_zMin = highKey;
			//	nNexSubQuery.m_zMin.m_nZValue += 1;
				TQuery nNexSubQuery;
				nNexSubQuery.m_zMin = m_CurrentSpatialQuery.m_zMin;
				nNexSubQuery.m_zMax = m_CurrentSpatialQuery.m_zMax;
				nNexSubQuery.m_zMin.clearLowBits (m_CurrentSpatialQuery.m_nBits);
		
				m_CurrentSpatialQuery.m_zMax.setLowBits(m_CurrentSpatialQuery.m_nBits);

				nNexSubQuery.m_nBits = --m_CurrentSpatialQuery.m_nBits;
				nNexSubQuery.m_ID = ++m_ID;
	
	
				if(nNexSubQuery.m_zMin > nNexSubQuery.m_zMax)
				{
					int dd =0;
					dd++;
				}
				if(m_CurrentSpatialQuery.m_zMin > m_CurrentSpatialQuery.m_zMax)
				{
					int dd =0;
					dd++;
				}

			//	bool bCheck1 = bCheckBit(nNexSubQuery.m_nBits, nNexSubQuery.m_zMin.m_nZValue, nNexSubQuery.m_zMax.m_nZValue);
			//	bool bCheck2 = bCheckBit(m_CurrentSpatialQuery.m_nBits, m_CurrentSpatialQuery.m_zMin.m_nZValue, m_CurrentSpatialQuery.m_zMax.m_nZValue);

			/*	if(!bCheck1 || !bCheck2)
				{
					int dd =0;
					dd++;
				}*/

				m_Queries.push(nNexSubQuery);
				//vecQuery.push_back(zInfo(nNexSubQuery.m_zMin.m_nZValue, nNexSubQuery.m_zMax.m_nZValue));
			}
			int dd = 0;
			dd++;
		}

	public:
		TBTree *m_pTree;
		TBTreeNode *m_pCurNode;
		TBTreeLeafNode*	m_pCurLeafNode;
		int32 m_nIndex;
		TPointKey m_zMin;
		TPointKey m_zMax;
		TRect m_QueryRect;
		TQuery m_CurrentSpatialQuery;
		typedef TSimpleStack<TQuery> TSpatialQueries;
		TSpatialQueries m_Queries;
		int m_ID;
		int64 m_nReq;
	};
}

#endif