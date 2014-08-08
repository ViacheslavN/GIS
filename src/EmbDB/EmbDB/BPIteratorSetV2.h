#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_ITERATOR_V2_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_ITERATOR_V2_H_
#include "BPTreeNodeSetV2.h"

namespace embDB
{



	template <class _TKey,	class _TLink, class _TComp, class _Transaction,	class _TInnerCompess, 	class _TLeafCompess >
	class TBPlusTreeSetV2;

	template <class _TKey,  class _TLink, class _TComp, class _Transaction, class _TInnerCompess, class _TLeafCompess>
	class TBPSetIteratorV2
	{
	public:
		//typedef _Traits Traits;
		typedef _TKey      TKey;
		typedef _TComp	   TComp;
		typedef _TLink     TLink;
		typedef _Transaction  Transaction;
		typedef _TInnerCompess TInnerCompess;  
		typedef _TLeafCompess TLeafCompess; 
		typedef  TBPVector<TKey> TLeafMemSet;
		typedef BPTreeNodeSetv2<TKey, TLink, TComp, Transaction, TInnerCompess, TLeafCompess> TBTreeNode;
		typedef BPTreeLeafNodeSetv2<TKey, TLink, TComp,	Transaction, TLeafCompess> TBTreeLeafNode;
		typedef TBPlusTreeSetV2<TKey,  TLink, TComp, Transaction, TInnerCompess, TLeafCompess> TBTree;

		TBPSetIteratorV2(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex) :
		m_pTree(pTree), m_pCurNode(pCurNode),  m_nIndex(nIndex)
		{		
			if(m_pCurNode)
			{
				assert(m_pCurNode->isLeaf());
				m_pCurNode->setFlags(BUSY_NODE, true);
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}


		}

		TBPSetIteratorV2(const TBPSetIteratorV2& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = nIndex;
			if(m_pCurNode)
			{
				m_pCurNode->setFlag(BUSY_NODE, true);
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}

		}
		~TBPSetIteratorV2()
		{
			if(m_pCurNode)
				m_pCurNode->setFlags(BUSY_NODE, false);
		}
		TBPSetIteratorV2& operator = (const TBPSetIteratorV2& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = nIndex;
			if(m_pCurNode)
			{
				m_pCurNode->setFlag(BUSY_NODE, true);
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}
			return this;
		}

		const TKey& key() const
		{
			return m_pCurLeafNode->key((uint32)m_nIndex);
		}
		TKey& key()
		{
			return m_pCurLeafNode->key((uint32)m_nIndex);
		}
		bool isNull()
		{
			if(m_pCurNode == NULL)
				return true;
			if(m_nIndex == - 1 && m_pCurNode->prev() == -1)
				return true;

			if((uint32)m_nIndex >= m_pCurLeafNode->count() && m_pCurNode->next() == -1)
				return true;
			
			 return false;

		}

		bool next()
		{
			if(isNull())
				return false;
			m_pTree->ClearChache();
			m_nIndex++;
			if((uint32)m_nIndex < m_pCurLeafNode->count())
				return true;

			if(m_pCurNode->next() != -1)
			{
				TBTreeNode *pNode = m_pTree->getNode(m_pCurNode->next()); //TO DO set busy
				m_pCurNode->setFlags(BUSY_NODE, false); 
				if(!pNode)
				{
					//to do log error
					return false;
				}
				m_nIndex = 0;
				m_pCurNode = pNode;
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
				return true;
			}
			return false;
		}
		void update()
		{
			assert(!isNull());
			m_pCurNode->setFlags(CHANGE_NODE, true);
		}

		bool back()
		{
			if(isNull())
				return false;
			m_pTree->ClearChache();
			m_nIndex--;
			if(m_nIndex >= 0)
				return true;

			if(m_pCurNode->prev() != -1)
			{
				TBTreeNode *pNode = m_pTree->getNode(m_pCurNode->prev()); //TO DO set busy
				m_pCurNode->setFlags(BUSY_NODE, false); 
				if(!pNode)
				{
					//to do log error
					return false;
				}
				
				m_pCurNode = pNode;
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
				m_nIndex = m_pCurLeafNode->count() - 1;
				return true;
			}
			return false;
		}
	private:
		TBTree *m_pTree;
		TBTreeNode *m_pCurNode;
		TBTreeLeafNode*	m_pCurLeafNode;
		int32 m_nIndex;
	};
}

#endif