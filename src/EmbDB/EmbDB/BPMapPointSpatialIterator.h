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
			m_nNodeCnt = 0;
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
				//	zMinPage.getXY(m_CurrentSpatialQuery.m_Rect.m_minX, m_CurrentSpatialQuery.m_Rect.m_minY);
				m_CurrentSpatialQuery.m_zMin = zMinPage;
				//	m_CurrentSpatialQuery.InitFromZOrder();
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
					return false;
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
			++m_nNodeCnt;
			if(!m_pCurNode)
				return false;
			/*if(m_zMax < m_pCurNode->key(m_nIndex))
			{
				m_pCurNode = NULL;
				return false;
			}*/
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
						CreateSubQuery();
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
				if(pParentNode->less() == m_pCurNode->addr())
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
			TPointKey highKey = m_CurrentSpatialQuery.m_zMax;
			TQuery curQuery = m_CurrentSpatialQuery;
			//TRect nRectLast;
			//zPageLast.getXY(nRectLast.m_minX, nRectLast.m_minY, nRectLast.m_maxX, nRectLast.m_maxY);
			short nBeginCurBit = m_CurrentSpatialQuery.m_nBits;
			TQuery BegincurQuery = m_CurrentSpatialQuery;
			while (zPageLast < highKey)
			{
			
				short nCurBit = curQuery.m_nBits;
				//	nCurBit = 63;

				/*if(curQuery.m_nBits < 0)
				{
					nCurBit = 63;
				}*/
				
				assert(nCurBit >= 0);
				while (curQuery.m_zMin.getBit (nCurBit) == curQuery.m_zMax.getBit (nCurBit) && nCurBit > 0)
				{
					if(nCurBit == 0)
					{
						int ndd = 0;
						ndd++;
					}
					nCurBit--;
					assert(nCurBit >= 0);
				}
				
				TPointKey lowKey = curQuery.m_zMin;
				highKey = curQuery.m_zMax;
				highKey.setLowBits(nCurBit);
				lowKey.clearLowBits (nCurBit);
			
				
				curQuery.m_nBits = --nCurBit;
				if(curQuery.m_nBits <= 0)
				{
					int ndd = 0;
					ndd++;
				}
			
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

		/*void SetCurrZval(std::set<uint64>& m_Zal, std::set<uint64>& m_OID)
		{
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;

			TLeafMemSet::iterator it = leafMemSet.begin();
			while(!it.isNull())
			{
				m_Zal.insert(it.key().m_nZValue);
				m_OID.insert(it.value());
				it.next();
			}
		}*/
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
		int64 m_nNodeCnt;
	};
}

#endif