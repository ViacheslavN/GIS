#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_ITERATOR_V2_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_ITERATOR_V2_H_
#include "BPTreeNodeSetV2.h"

namespace embDB
{



	template <class _TKey,	 class _TComp, class _Transaction,	class _TInnerCompess, 	class _TLeafCompess ,
	class _TInnerNode, 	class _TLeafNode, class _TBTreeNode>
	class TBPlusTreeSetV2;

	template <class _TKey,  class _TComp, class _Transaction, class _TInnerCompess, class _TLeafCompess, 
	class _TInnerNode, 	class _TLeafNode, class _TBTreeNode>
	class TBPSetIteratorV2
	{
	public:
		//typedef _Traits Traits;
		typedef _TKey      TKey;
		typedef _TComp	   TComp;
		typedef int64     TLink;
		typedef _Transaction  Transaction;
		typedef _TInnerCompess TInnerCompess;  
		typedef _TLeafCompess TLeafCompess; 
		typedef _TInnerNode	TInnerNode;
		typedef _TLeafNode TBTreeLeafNode;
		typedef _TBTreeNode TBTreeNode;
		typedef IRefCntPtr<TBTreeNode> TBTreeNodePtr;

		typedef  TBPVector<TKey> TLeafMemSet;
		/*typedef BPTreeNodeSetv2<TKey, TComp, Transaction, TInnerCompess, TLeafCompess, TInnerNode, TLeafNode, TBTreeNode> TBTreeNode;
		typedef BPTreeLeafNodeSetv2<TKey, TComp,	Transaction, TLeafCompess> TBTreeLeafNode;*/
		typedef TBPlusTreeSetV2<TKey,  TComp, Transaction, TInnerCompess, TLeafCompess
		, TInnerNode, TBTreeLeafNode, TBTreeNode> TBTree;

		TBPSetIteratorV2(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex) :
		m_pTree(pTree), m_pCurNode(pCurNode),  m_nIndex(nIndex)
		{		
			if(m_pCurNode.get())
			{
				assert(m_pCurNode->isLeaf());
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}


		}

		TBPSetIteratorV2() : m_pCurNode(NULL), m_pTree(NULL), m_nIndex(0)
		{

		}

		TBPSetIteratorV2(const TBPSetIteratorV2& iter)
		{
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
			if(m_pCurNode.get())
			{
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}

		}
		~TBPSetIteratorV2()
		{
 
		}
		 TBPSetIteratorV2& operator = (const TBPSetIteratorV2& iter)
		{
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;;
			m_pCurLeafNode = NULL;
			if(m_pCurNode.get())
			{
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}
			return *this;
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
			if(m_pCurNode.get() == NULL || m_nIndex == - 1)
				return true;
	
			return false;

		}

		bool next()
		{
			if(isNull())
				return false;
			//m_pTree->ClearChache();
			m_nIndex++;
			if((uint32)m_nIndex < m_pCurLeafNode->count())
				return true;

			if(m_pCurNode->next() != -1)
			{
				TBTreeNodePtr pNode = m_pTree->getNode(m_pCurNode->next()); //TO DO set busy
				m_pCurNode->setFlags(BUSY_NODE, false); 
				if(!pNode.get())
				{
					//to do log error
					return false;
				}
				m_pTree->SetParentNext(m_pCurNode.get(), pNode.get());
				m_nIndex = 0;
				m_pCurNode = pNode;
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
				return true;
			}
			m_pCurNode = NULL;
			m_nIndex = -1;
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
			//m_pTree->ClearChache();
			m_nIndex--;
			if(m_nIndex >= 0)
				return true;

			if(m_pCurNode->prev() != -1)
			{
				TBTreeNodePtr pNode = m_pTree->getNode(m_pCurNode->prev()); //TO DO set busy
				if(!pNode.get())
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
	public:
		TBTree *m_pTree;
		TBTreeNodePtr m_pCurNode;
		TBTreeLeafNode*	m_pCurLeafNode;
		int32 m_nIndex;
	};
}

#endif