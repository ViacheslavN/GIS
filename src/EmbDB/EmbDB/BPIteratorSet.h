#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_ITERATOR_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_ITERATOR_H_
#include "BPTreeNodeSet.h"

namespace embDB
{



	template <class _TKey,  class _TLink, class _TComp, class _Transaction, class _TInnerMemSet, 
		class _TLeafMemSet, class _TInnerCompess,	class _TLeafCompess ,class _TInnerNode , class _TLeafNode, class _TBTreeNode>
	class TBPlusTreeSet;
	
	template <class _TKey,  class _TLink, class _TComp, class _Transaction,  class _TInnerMemSet, 
	class _TLeafMemSet, class _TInnerCompess,	class _TLeafCompess ,class _TInnerNode , class _TLeafNode, class _TBTreeNode>
	class TBPSetIterator
	{
	public:
		//typedef _Traits Traits;
		typedef _TKey      TKey;
		typedef _TComp	   TComp;
		typedef _TLink     TLink;
		typedef _Transaction  Transaction;
		typedef _TBTreeNode  TBTreeNode;
		typedef _TInnerMemSet TInnerMemSet;  
		typedef _TLeafMemSet TLeafMemSet; 
		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeftMemSetNode;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode TLeafNode;
		typedef _TInnerCompess TInnerCompess;  
		typedef _TLeafCompess TLeafCompess; 
		typedef TBPlusTreeSet<TKey,  TLink, TComp, Transaction, TInnerMemSet, 
			TLeafMemSet, TInnerCompess,	TLeafCompess ,TInnerNode , TLeafNode,  TBTreeNode> TBTree;

		TBPSetIterator(TBTree *pTree, TBTreeNode *pCurNode, TLeftMemSetNode* pRBNode) :
		m_pTree(pTree), m_pCurNode(pCurNode), m_pRBNode(pRBNode)
		{		
			if(m_pCurNode)
			{
				assert(m_pCurNode->isLeaf());
				m_pCurNode->setFlags(BUSY_NODE, true);
				if(!m_pRBNode)
				{
					TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;
					m_pRBNode = leafMemSet.tree_minimum(leafMemSet.root());
				}
			}


		}

		TBPSetIterator(const TBPSetIterator& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_pRBNode = iter.m_pRBNode;

		}
		~TBPSetIterator()
		{
			if(m_pCurNode)
				m_pCurNode->setFlags(BUSY_NODE, false);
		}
		TBPSetIterator& operator = (const TBPSetIterator& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_pRBNode = iter.m_pRBNode;
			return this;
		}

		const TKey& key() const
		{
			return m_pRBNode->m_key;
		}
		TKey& key()
		{
			return m_pRBNode->m_key;
		}
		bool isNull()
		{
			if(m_pCurNode == NULL || m_pRBNode == NULL)
				return true;
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;
			return leafMemSet.isNull(m_pRBNode);
		}

		bool next()
		{
			if(!m_pCurNode)
				return false;
			m_pTree->ClearChache();
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;
			if(leafMemSet.isNull(m_pRBNode->m_pNext))
			{
				m_pRBNode = NULL;
				TBTreeNode *pNode = m_pTree->getNode(m_pCurNode->next()); //������� ��������� ��������� ����� ����������  ������
				m_pCurNode->setFlags(BUSY_NODE, false); 
				if(!pNode)
				{
					m_pCurNode = NULL;
					return false;

				}
				else
				{
					assert(pNode->isLeaf());
					pNode->setFlags(BUSY_NODE, true); 
					m_pCurNode = pNode;
					TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;
					m_pRBNode = leafMemSet.tree_minimum(leafMemSet.root());
					if(leafMemSet.isNull(m_pRBNode))
						return false;
				}
			}
			else
				m_pRBNode = m_pRBNode->m_pNext;

			return true;
		}
		void update()
		{
			assert(!isNull());
			m_pCurNode->setFlags(CHANGE_NODE, true);
		}
	private:
		TBTree *m_pTree;
		TBTreeNode *m_pCurNode;
		TLeftMemSetNode* m_pRBNode;
	};
}

#endif