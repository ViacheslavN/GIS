#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_POINT_ITERATOR_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SPATIAL_POINT_ITERATOR_MAP_H_
//#include "PointSpatialBPMaTraits.h"
#include "BPTreeNodeMap.h"
#include "CommonLibrary/SpatialKey.h"
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
		typedef CommonLib::TRect2D<TPointType>         TRect;
		typedef CommonLib::IRefCntPtr<TBTreeNode> TBTreeNodePtr;
		TComp m_comp;

		typedef TBPPointSpatialMap<_TCoord, _TValue, _TComp, _Transaction,	_TInnerCompess ,_TLeafCompess,
		 _TInnerNode ,	_TLeafNode, _TBTreeNode> TBTree;



 
		bool FindRectMinZVal(const TPointKey& zVal, 
			const TPointKey& zMin, const TPointKey& zMax, TPointKey& zRes)
		{
			if(zVal < zMin || zVal > zMax)
				return false;

			short nBits = zRes.getBits();

			TPointKey left = zMin;
			TPointKey right = zMax;
			zRes = zMax;
			while(nBits >= 0)
			{


				TPointKey qMin = left;
				TPointKey qMax = right;

				while (qMin.getBit (nBits) == qMax.getBit (nBits))
				{

					nBits--;
					assert(nBits >= 0);
		
				}
				qMin.clearLowBits(nBits);
				qMax.setLowBits(nBits);

				if(zVal < qMax)
				{
					right = qMax;
					zRes = qMax;
				}
				else 
				{
					if(qMin > zVal)
					{
						zRes = qMin;
						break;
					}
					left = qMin;
					zRes = qMin;

				}
				nBits--;
			}

			return true;
		}


		TBPTSpatialPointIteratorMap(TBTree *pTree) :
			m_pTree(pTree), m_pCurNode(NULL), m_pCurLeafNode(NULL)
		{}


		TBPTSpatialPointIteratorMap(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex, 
			TPointKey& zMin, TPointKey& zMax, TRect& QueryRect) : 
		m_pTree(pTree), m_zMin(zMin), m_zMax(zMax), m_QueryRect(QueryRect), m_nIndex(nIndex)
		{		
			m_nReq = 0;
			
			m_pCurNode = pCurNode;
			SetNode(m_pCurNode.get());
		}

		void SetNode(TBTreeNode *pCurNode)
		{
		
			if(m_pCurNode.get() && m_nIndex != -1)
			{
				assert(m_pCurNode->isLeaf());
			 	m_pCurLeafNode = &m_pCurNode->m_LeafNode;
							
			}
		}

		TBPTSpatialPointIteratorMap(const TBPTSpatialPointIteratorMap& iter)
		{

			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
			m_pCurLeafNode = iter.m_pCurLeafNode;

		}
		~TBPTSpatialPointIteratorMap()
		{
	 
		}
		TBPTSpatialPointIteratorMap& operator = (const TBPTSpatialPointIteratorMap& iter)
		{
 
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
			m_pCurLeafNode = iter.m_pCurLeafNode;
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
			if(m_pCurNode.get() == NULL || m_nIndex == - 1)
				return true;
					
			TPointKey& zPage = m_pCurNode->key(m_nIndex);
			if(m_zMax < zPage)
				return true;
			return false;
		}
		bool CheckKey( TPointKey& zVal )
		{
			
		}
		bool findNext( const TPointKey& zNextVal, int64 nAddr )
		{
			if(zNextVal > m_zMax)
				return false;

				if(zNextVal.IsInRect(m_QueryRect))
				{
					m_nIndex = 0;
					m_pCurNode  = m_pTree->getNode(nAddr, false, false, true); 
					SetNode(m_pCurNode.get());
					return true;
						
				}
				TPointKey zRes;
				FindRectMinZVal(zNextVal, m_zMin, m_zMax, zRes);
				TBTree::iterator it = m_pTree->lower_bound(zRes);
				if(it.isNull())
					return false;

				m_pCurNode = it.m_pCurNode;
				m_nIndex = it.m_nIndex;
				SetNode(m_pCurNode.get());
				return true;

		}

		bool next()
		{
			if(!m_pCurNode.get())
				return false;
		
			m_pTree->ClearChache();
			m_nIndex++;
			while(m_nIndex <  (int32)m_pCurLeafNode->count())
			{
				TPointKey& zVal = m_pCurNode->key(m_nIndex);
				if(zVal.IsInRect(m_QueryRect))
					break;
				m_nIndex++;
			}

			if(m_nIndex < (int32)m_pCurLeafNode->count())
			   return true;

 

			TBTreeNodePtr pParentNode = m_pTree->getNode(m_pCurNode->parentAddr(), false, false, true);
			if(/*pParentNode.get() && */(m_pCurNode->foundIndex()  == -1 || (m_pCurNode->foundIndex() + 1) < (int32)pParentNode->count()))
			{
				int nIndex = m_pCurNode->foundIndex()  == -1 ? 0 : m_pCurNode->foundIndex() + 1;
				TBTreeNodePtr pNextNode = m_pTree->getNode(pParentNode->link(nIndex), false, false, true);
				
				TPointKey& zNextVal = pParentNode->key(nIndex);

				 pNextNode->setParent(pParentNode.get(), nIndex);
				return findNext(zNextVal, pNextNode->addr());
			}
			else
			{ 
				TBTreeNodePtr pNextNode = m_pTree->getNode(m_pCurNode->next(), false, false, true);
				if(!pNextNode.get())
					return false;
				m_pTree->SetParentNext(m_pCurNode.get(), pNextNode.get());
				 return findNext(pNextNode->key(0), pNextNode->addr());
			}

			return true;
		}
		void update()
		{
			assert(!isNull());
			m_pCurNode->setFlags(CHANGE_NODE, true);
		}

	

	public:
		TBTree *m_pTree;
		TBTreeNodePtr m_pCurNode;
		TBTreeLeafNode*	m_pCurLeafNode;
		int32 m_nIndex;
		TPointKey m_zMin;
		TPointKey m_zMax;
		TRect m_QueryRect;
		int64 m_nReq;
	};
}

#endif