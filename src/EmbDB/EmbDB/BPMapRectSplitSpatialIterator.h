#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_RECT_SPLIT_ITERATOR_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_RECT_SPLIT_ITERATOR_H_
//#include "PointSpatialBPMaTraits.h"
#include "CommonLibrary/SpatialKey.h"
#include "simple_stack.h"
namespace embDB
{
	template<class _TCoord, class _TValue, class _TComp, 
	class _Transaction,	class _TInnerCompess ,	class _TLeafCompess,
	class _TInnerNode ,	class _TLeafNode, class _TBTreeNode>
	class TBPPointSpatialMap;


	template<class TZOrder>
	class SubQuery
	{
	public:
			TZOrder m_zMin;
			TZOrder m_zMax;
			short m_nBits;
		/*	TRect minRect;
			TRect maxRect;
			short m_nBits;
			short m_ID;*/



			SubQuery() : m_nBits(0)
			{

			}

			/*void FromZ()
			{
				m_zMin.getXY(minRect.m_minX, minRect.m_minY, minRect.m_maxX, minRect.m_maxY);
				m_zMax.getXY(maxRect.m_minX, maxRect.m_minY, maxRect.m_maxX, maxRect.m_maxY);
			}*/
		};



	template <class _TCoord, class _TValue, class _TComp, 		class _Transaction, class _TInnerCompess ,	class _TLeafCompess,
	class _TInnerNode, class _TLeafNode,  class _TBTreeNode>
	class TBPTSpatialRectSplitIterator
	{
	public:

		typedef  _TCoord     TPointKey;
		typedef typename TPointKey::TPointType  TPointType;
		typedef _TValue    TValue;
		typedef  _TComp	   TComp;
		typedef	 _Transaction  Transaction;
		typedef _TLeafNode TBTreeLeafNode;
		typedef  _TBTreeNode TBTreeNode;
		typedef CommonLib::TRect2D<TPointType>         TRect;
		typedef CommonLib::IRefCntPtr<TBTreeNode> TBTreeNodePtr;
		TComp m_comp;

		typedef TBPPointSpatialMap<_TCoord, _TValue, _TComp, _Transaction,	_TInnerCompess ,_TLeafCompess,
			_TInnerNode ,	_TLeafNode, _TBTreeNode> TBTree;

		 

		


		TBPTSpatialRectSplitIterator(TBTree *pTree) :
		m_pTree(pTree), m_pCurNode(NULL), m_pCurLeafNode(NULL)
		{}


		TBPTSpatialRectSplitIterator(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex, 
			TPointKey& zMin, TPointKey& zMax, TRect& QueryRect) : 
		m_pTree(pTree), m_zMin(zMin), m_zMax(zMax), m_QueryRect(QueryRect), m_nIndex(nIndex)
		{		

			m_CurrentSpatialQuery.m_zMin = zMin;
			m_CurrentSpatialQuery.m_zMax = zMax;
			m_CurrentSpatialQuery.m_nBits = m_zMin.getBits();
			m_pCurNode = pCurNode;
			SetNode(m_pCurNode.get());
			if(m_nIndex != -1)
			{
				CreateSubQuery();
				CheckIndex();
			}
		}

		void SetNode(TBTreeNode *pCurNode)
		{

			if(m_pCurNode.get() && m_nIndex != -1)
			{
				assert(m_pCurNode->isLeaf());
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;

			}
		}

		TBPTSpatialRectSplitIterator(const TBPTSpatialRectSplitIterator& iter)
		{

			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
			m_pCurLeafNode = iter.m_pCurLeafNode;
			m_zMax = iter.m_zMax;
			m_zMin = iter.m_zMin;
			m_QueryRect = iter.m_QueryRect;
			m_CurrentSpatialQuery = iter.m_CurrentSpatialQuery;
			m_Queries = iter.m_Queries;

		}
		~TBPTSpatialRectSplitIterator()
		{

		}
		TBPTSpatialRectSplitIterator& operator = (const TBPTSpatialRectSplitIterator& iter)
		{

			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
			m_pCurLeafNode = iter.m_pCurLeafNode;
			m_zMax = iter.m_zMax;
			m_zMin = iter.m_zMin;
			m_CurrentSpatialQuery = iter.m_CurrentSpatialQuery;
			m_Queries = iter.m_Queries;
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

		bool isNull() const
		{
			if(m_pCurNode.get() == NULL || m_nIndex == - 1)
				return true;

			if(m_nIndex < (int32)m_pCurNode->count())
			{
				const TPointKey& zPage = m_pCurNode->key(m_nIndex);
				if(zPage < m_zMax)
					return false;
			}

			return true;
		}
		bool CheckKey( TPointKey& zVal )
		{

		}



		bool CheckIndex()
		{
			if(m_nIndex == -1)
				return false;

			while(true)
			{

				while(m_nIndex <  (int32)m_pCurLeafNode->count())
				{

					TPointKey& zVal = m_pCurNode->key(m_nIndex);

					if(m_CurrentSpatialQuery.m_zMax < zVal)
						break;

					if(zVal.IsInRect(m_QueryRect))
					{						
						return true;
					}
					m_nIndex++;
				}
				if(m_nIndex == (int32)m_pCurLeafNode->count() - 1)
				{

					TPointKey& zVal = m_pCurNode->key(m_nIndex);
					if(zVal.IsInRect(m_QueryRect))
					{						
						return true;
					}
					m_nIndex++;
				}

				if(m_Queries.empty())
				{
					m_nIndex == -1;
					return false;
				}

				if(m_nIndex == (int32)m_pCurLeafNode->count())
				{
					if(m_pCurNode->next() == -1)
					{
						m_nIndex = -1;
						return false;
					}

					if(!findNext(true))
						return false;

				}			
				else
				{
					m_CurrentSpatialQuery = m_Queries.top();

					short nType = 0;
					int32 nIndex = m_pCurNode->leaf_lower_bound(m_pTree->getComp(), m_CurrentSpatialQuery.m_zMin,  nType);
					if(nIndex != -1)
					{
						m_nIndex = nIndex;
						CreateSubQuery();
					}
					else
					{
						if(!findNext(false))
							return false;
					}

				}

			}
		}

		bool findNext(bool bNextQuery = true)
		{
			while(true)
			{

				if(bNextQuery)
				{
					if(m_Queries.empty())
					{
						m_nIndex = -1;
						return false;
					}
					m_CurrentSpatialQuery = m_Queries.top();
				}
				typename TBTree::iterator it = m_pTree->lower_bound(m_CurrentSpatialQuery.m_zMin);
				if(it.isNull())
				{
					if(m_Queries.empty())
					{
						m_nIndex = -1;
						return false;
					}
					else
					{
						bNextQuery = true;
						continue;
					}

				}
 

				m_pCurNode = it.m_pCurNode;
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
				m_nIndex = it.m_nIndex;
				CreateSubQuery();
				return true;
			}
	 
		}

		 

		bool next()
		{
			if(!m_pCurNode.get())
				return false;

			m_pTree->ClearChache();
			m_nIndex++;
			return CheckIndex();
		}
		void update()
		{
			assert(!isNull());
			m_pCurNode->setFlags(CHANGE_NODE, true);
		}

		int64 addr() const
		{
			if(isNull())
				return - 1;
			return m_pCurNode->addr();
		}
		int32 pos() const
		{
			return m_nIndex;
		}
		bool setAddr(int64 nAddr, int32  nPos)
		{

			if(nAddr == -1)
			{
				m_pCurLeafNode = NULL;
				m_pCurNode = NULL;
				return true;
			}

			TBTreeNodePtr pNode = m_pTree->getNode(nAddr); 
			if(!pNode.get())
			{
				//to do log error
				return false;
			}
			m_pCurNode = pNode;
			m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			m_nIndex = nPos;
			return true;
		}


		void CreateSubQuery()
		{
			if(m_pCurNode->count() == 0)
			{
				assert(false);
				return;
			}
			TPointKey zPageLast = m_pCurLeafNode->key(m_pCurLeafNode->count() - 1);
						
			while (zPageLast < m_CurrentSpatialQuery.m_zMax)
			{

		
				
				assert(m_CurrentSpatialQuery.m_nBits >= 0);
				while (m_CurrentSpatialQuery.m_zMin.getBit (m_CurrentSpatialQuery.m_nBits) == m_CurrentSpatialQuery.m_zMax.getBit (m_CurrentSpatialQuery.m_nBits))
				{
					
					m_CurrentSpatialQuery.m_nBits--;
					assert(m_CurrentSpatialQuery.m_nBits >= 0);
				}
				
				TSubQuery nNexSubQuery;
				nNexSubQuery.m_zMin = m_CurrentSpatialQuery.m_zMin;
				nNexSubQuery.m_zMax = m_CurrentSpatialQuery.m_zMax;
				nNexSubQuery.m_zMin.clearLowBits (m_CurrentSpatialQuery.m_nBits);
		
				m_CurrentSpatialQuery.m_zMax.setLowBits(m_CurrentSpatialQuery.m_nBits);

				if(m_CurrentSpatialQuery.m_nBits == 0)
				{
					int dd =1 ;
					dd++;
				}
				nNexSubQuery.m_nBits = --m_CurrentSpatialQuery.m_nBits;
				//nNexSubQuery.m_ID = ++m_ID;
	
	
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

				m_Queries.push(nNexSubQuery);
			}

		}

	public:


		typedef SubQuery<TPointKey> TSubQuery;
		TSubQuery m_CurrentSpatialQuery;
		typedef TSimpleStack<TSubQuery> TSpatialQueries;
		TSpatialQueries m_Queries;
		TRect m_QueryRect;

		TBTree *m_pTree;
		TBTreeNodePtr m_pCurNode;
		TBTreeLeafNode*	m_pCurLeafNode;
		int32 m_nIndex;
		TPointKey m_zMin;
		TPointKey m_zMax;
		
 

		

		//TComp m_comp;
	};
}

#endif