#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_POINT_ITERATOR_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_POINT_ITERATOR_MAP_H_
#include "PointSpatialBPMaTraits.h"
#include "BPTreeNodeMap.h"
#include "SpatialKey.h"
#include "simple_stack.h"
namespace embDB
{

	template <class _TCoord, class _TValue, class _TComp, 	class _TInnerCompess ,	class _TLeafCompess,
	class _Transaction>
	class TBPPointSpatialMap;


	template <class _TCoord, class _TValue, class _TComp, 	class _TInnerCompess ,	class _TLeafCompess,
	class _Transaction,	class _TBTreeNode, class _TLeftMemset,  class _TLeftNode>
	class TBPTSpatialPointIteratorMap
	{
	public:

		typedef  _TCoord     TPointKey;
		typedef typename TPointKey::TPointType  TPointType;
		typedef typename _TValue    TValue;
		typedef typename  _TComp	   TComp;
		typedef	typename  _Transaction  Transaction;
		typedef typename _TLeftMemset TLeafMemSet;
		typedef  _TBTreeNode TBTreeNode;
		typedef typename   TLeafMemSet::TTreeNode TLeftMemSetNode;
		typedef TRect2D<TPointType>         TRect;
		/*


	
		*/

		typedef TBPPointSpatialMap<_TCoord, _TValue,  _TComp, _TInnerCompess ,_TLeafCompess, _Transaction> TBTree;


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
			m_pTree(pTree), m_pCurNode(NULL), m_pRBNode(NULL)
		{}


		TBPTSpatialPointIteratorMap(TBTree *pTree, TBTreeNode *pCurNode, TLeftMemSetNode* pRBNode, 
			TPointKey& zMin, TPointKey& zMax, TRect& QueryRect) : 
		m_pTree(pTree), m_zMin(zMin), m_zMax(zMax), m_QueryRect(QueryRect)
		{		
			m_nReq = 0;
			m_nNodeCnt = 0;
			m_ID = 1;
			m_CurrentSpatialQuery.m_zMin = m_zMin;
			m_CurrentSpatialQuery.m_zMax = m_zMax;
			m_CurrentSpatialQuery.m_nBits = m_zMin.getBits();
			m_CurrentSpatialQuery.m_ID = m_ID;
			m_pCurNode = pCurNode;
			m_pRBNode = pRBNode;
			if(pCurNode && pRBNode)
			{
				SetParams(pCurNode, pRBNode);
				CreateSubQuery();
			}

			
		}

		void SetParams(TBTreeNode *pCurNode, TLeftMemSetNode* pRBNode)
		{
		
			if(m_pCurNode)
			{
				assert(m_pCurNode->isLeaf());
				m_pCurNode->setFlags(BUSY_NODE, true);
				if(m_pRBNode)
				{
					TPointKey& zMinPage = m_pRBNode->m_key;
				//	zMinPage.getXY(m_CurrentSpatialQuery.m_Rect.m_minX, m_CurrentSpatialQuery.m_Rect.m_minY);
					m_CurrentSpatialQuery.m_zMin = zMinPage;
				//	m_CurrentSpatialQuery.InitFromZOrder();

				}
			}
		}
		TBPTSpatialPointIteratorMap(const TBPTSpatialPointIteratorMap& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_pRBNode = iter.m_pRBNode;

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
			m_pRBNode = iter.m_pRBNode;
			return this;
		}

		const TPointKey& key() const
		{
			return m_pRBNode->m_key;
		}
		const TValue& value()  const
		{
			return m_pRBNode->m_val;
		}

		TPointKey& key()
		{
			return m_pRBNode->m_key;
		}
		TValue& value()
		{
			return m_pRBNode->m_val;
		}

		bool isNull()
		{
			if(m_pCurNode == NULL || m_pRBNode == NULL)
				return true;
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;
			if(leafMemSet.isNull(m_pRBNode))
				return true;
			TPointKey& zPage = m_pRBNode->m_key;
			if(m_zMax < zPage)
				return true;
			return false;
		}

		bool findNext(bool bNextQuery = true)
		{
			m_pRBNode = NULL;
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
				if(m_pTree->SpatialfindNode(m_CurrentSpatialQuery.m_zMin, &m_pCurNode, &m_pRBNode))
				{
					if(!m_pRBNode)
						continue;
					if(!m_pCurNode)
						continue;
					if(m_pRBNode->m_key > m_CurrentSpatialQuery.m_zMax)
						continue;
					m_pCurNode->setFlags(CHANGE_NODE, true);
					SetParams(m_pCurNode, m_pRBNode);
					CreateSubQuery();
					return true;
				}

			}
		}

		bool next()
		{
			++m_nNodeCnt;
			if(!m_pCurNode)
				return false;
			if(m_zMax < m_pRBNode->m_key)
			{
				m_pCurNode = NULL;
				return false;
			}
			m_pTree->ClearChache();
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;

			uint32 nSize = leafMemSet.size();
			if(leafMemSet.isNull(m_pRBNode->m_pNext))
			{
				m_nNodeCnt = 0;
				if(m_pCurNode->m_bValidNextKey)
				{
					if(m_pCurNode->m_NextLeafKey == m_pRBNode->m_key )
					{
						m_pCurNode->setFlags(BUSY_NODE, false); 
						m_pCurNode = m_pTree->getNode(m_pCurNode->next(), false, false, true);
						m_pCurNode->setFlags(BUSY_NODE, true); 
						assert(m_pCurNode != NULL);
						m_pRBNode = m_pCurNode->firstLeftMemSetNode();
						assert(!m_pCurNode->isLeamMemsetNodeNull(m_pRBNode));
						return true;
					}
					else
					{
						m_pRBNode = NULL;
						TBTreeNode *pBPNode = m_pTree->getNode(m_pCurNode->next(), false, false, true);
						if(!pBPNode)
						{
							m_pCurNode->setFlags(BUSY_NODE, false); 
							m_pRBNode = NULL;
							return findNext();
						}
						return findNext();
						TLeftMemSetNode* pFirstRBNode = pBPNode->firstLeftMemSetNode();
						while(m_Queries.size())
						{
							m_CurrentSpatialQuery = m_Queries.top();
							if(pFirstRBNode->m_key < m_CurrentSpatialQuery.m_zMax )
								break;
							
						}
						return findNext(false);
					}
				}
				else
				{
					//все ровно придется проверять
					TBTreeNode *pBPNode = m_pTree->getNode(m_pCurNode->next(), false, false, true);
					if(!pBPNode)
					{
						m_pCurNode->setFlags(BUSY_NODE, false); 
						m_pRBNode = NULL;
						return findNext();
					}
					pBPNode->setFlags(BUSY_NODE, true); 
					TLeftMemSetNode* pRBNode = pBPNode->firstLeftMemSetNode();
					assert(!pBPNode->isLeamMemsetNodeNull(pRBNode));
					if(pRBNode->m_key  == m_pRBNode->m_key )
					{
						m_pCurNode->setFlags(BUSY_NODE, false); 
						m_pCurNode = pBPNode;
						m_pRBNode = pRBNode;
						return true;
					}
					else
					{
						return findNext();
						while(m_Queries.size())
						{
							m_CurrentSpatialQuery = m_Queries.top();
							if(pRBNode->m_key < m_CurrentSpatialQuery.m_zMax )
								break;

						}
						pBPNode->setFlags(BUSY_NODE, false); 
						m_pRBNode = NULL;
						return findNext(false);
					}
				}
				m_pRBNode = NULL;
				return findNext();
			}	


			m_pRBNode = m_pRBNode->m_pNext;
			TPointKey& zPage = m_pRBNode->m_key;
			if(m_zMax < zPage)
			{
				m_pCurNode = NULL;
				return false;
			}
			if( m_CurrentSpatialQuery.m_zMax < m_pRBNode->m_key)
			{
				if(m_Queries.empty())
				{
					m_pCurNode = NULL;
					return false;
				}
				m_CurrentSpatialQuery = m_Queries.top();
			/*	if(zPage < m_CurrentSpatialQuery.m_zMin )
				{
					m_Queries.push(m_CurrentSpatialQuery);
					return true;
				}*/
		

				m_pCurNode->setFlags(BUSY_NODE, false);
				TLeftMemSetNode *pRBNode = m_pCurNode->findLessOrEQNode(m_CurrentSpatialQuery.m_zMin, m_pRBNode);
				if(pRBNode)
				{
					m_pRBNode = pRBNode;
					if(m_pRBNode->m_key > m_CurrentSpatialQuery.m_zMax)
					{
						m_nReq++;
						return findNext(false);
					}
					m_pCurNode->setFlags(BUSY_NODE, true);
					CreateSubQuery();
					if(m_pRBNode->m_key > m_CurrentSpatialQuery.m_zMax)
					{
						m_nReq++;
						return findNext(false);
					}
					return true;
				}
				else 
				{
					return findNext(false);
				}
			}
			return true;
		}
		void update()
		{
			assert(!isNull());
			m_pCurNode->setFlags(CHANGE_NODE, true);
		}
		void CreateSubQuery(TLeftMemSetNode * pLastMemSet = NULL)
		{
			if(m_pCurNode->count() == 0)
			{
				assert(false);
				return;
			}
			//TPointKey zPageLast(m_currentNodePageRect.m_maxX, m_currentNodePageRect.m_maxY);
			if(!pLastMemSet)
				 pLastMemSet = m_pCurNode->lastLeftMemSetNode();
			TPointKey zPageLast = pLastMemSet->m_key;
			TPointKey highKey = m_CurrentSpatialQuery.m_zMax;
			TQuery curQuery = m_CurrentSpatialQuery;
			TRect nRectLast;
			zPageLast.getXY(nRectLast.m_minX, nRectLast.m_minY, nRectLast.m_maxX, nRectLast.m_maxY);
		
			while (zPageLast < highKey)
			{
				short nCurBit = curQuery.m_nBits;
				assert(nCurBit >= 0);
				while (curQuery.m_zMin.getBit (nCurBit) == curQuery.m_zMax.getBit (nCurBit) && nCurBit > 0)
				{
					nCurBit--;
					assert(nCurBit >= 0);
				}
				
				TPointKey lowKey = curQuery.m_zMin;
				highKey = curQuery.m_zMax;
				highKey.splitByBits(nCurBit);
				lowKey.clearLowBits (nCurBit);
			
				curQuery.m_nBits = --nCurBit;


				TQuery nLeftQuery;// запрос с меньшими ключами
				nLeftQuery.m_zMin = curQuery.m_zMin;
				nLeftQuery.m_zMax = highKey;
				nLeftQuery.m_nBits = curQuery.m_nBits;
				nLeftQuery.m_ID = ++m_ID;
				nLeftQuery. FromZ();

 
			/*	if(nLeftQuery.m_zMin > nLeftQuery.m_zMax)
				{
					int dd =0;
					dd++;
				}*/

				TQuery nRightQuery;// запрос с большими ключами
				nRightQuery.m_zMin = lowKey;
				nRightQuery.m_zMax = curQuery.m_zMax;
				nRightQuery.m_nBits = curQuery.m_nBits;
				nRightQuery.m_ID = ++m_ID;
				nRightQuery. FromZ();

			/*	if(nRightQuery.m_zMin > nRightQuery.m_zMax)
				{
					int dd =0;
					dd++;
				}


				if(nLeftQuery.m_zMin > nRightQuery.m_zMin || nLeftQuery.m_zMin > nRightQuery.m_zMax || nLeftQuery.m_zMax > nRightQuery.m_zMin || nLeftQuery.m_zMax > nRightQuery.m_zMax )
				{
					int dd =0;
						dd++;
				}*/
							
				curQuery = nLeftQuery; 
				m_Queries.push(nRightQuery);
			}
			m_CurrentSpatialQuery = curQuery;
		//	m_CurrentSpatialQuery.m_zMax = zPageLast;
		}

		void SetCurrZval(std::set<uint64>& m_Zal, std::set<uint64>& m_OID)
		{
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;

			TLeafMemSet::iterator it = leafMemSet.begin();
			while(!it.isNull())
			{
				m_Zal.insert(it.key().m_nZValue);
				m_OID.insert(it.value());
				it.next();
			}
		}
	public:
		TBTree *m_pTree;
		TBTreeNode *m_pCurNode;
		TLeftMemSetNode* m_pRBNode;
		TPointKey m_zMin;
		TPointKey m_zMax;
		TRect m_QueryRect;
		TQuery m_CurrentSpatialQuery;
		typedef TSimpleStack<TQuery> TSpatialQueries;
		TSpatialQueries m_Queries;
		int m_ID;
		int64 m_nReq;
		int64 m_nNodeCnt;
	};
}

#endif