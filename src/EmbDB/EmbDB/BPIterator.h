#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_ITERATOR_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_ITERATOR_H_
#include "BPTreeNode.h"

namespace embDB
{

	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction, class _TInnerComp, class _TLeafComp, class _TBreeNode>
	class TBPlusTreeMap;


	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction, class _TInnerComp, class _TLeafComp,
		class _TBreeNode = BPTreeNode<_TKey, _TValue, _TLink, _TComp, _Transaction, _TInnerComp, _TLeafComp> >
	class TBPTIterator
	{
	public:
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef _TLink TLink;
		typedef _TComp TComp;
		typedef _TBreeNode  TBTreeNode;
		typedef	_Transaction  Transaction;
		typedef _TInnerComp TInnerCompressor;
		typedef _TLeafComp TLeafCompressor;
		typedef typename TBTreeNode::TLeafMemSet TLeafMemSet;
		typedef typename TLeafMemSet::TTreeNode TTreeNode;
				
		typedef TBPlusTreeMap<TKey, TValue, TLink, TComp, Transaction, TInnerCompressor, TLeafCompressor, TBTreeNode> TBTree;

		TBPTIterator(TBTree *pTree, TBTreeNode *pCurNode, TTreeNode* pRBNode) :
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

		TBPTIterator(const TBPTIterator& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_pRBNode = iter.m_pRBNode;

		}
		~TBPTIterator()
		{
			if(m_pCurNode)
				m_pCurNode->setFlags(BUSY_NODE, false);
		}
		TBPTIterator& operator = (const TBPTIterator& iter)
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
		const TValue& value()  const
		{
			return m_pRBNode->m_val;
		}

		TKey& key()
		{
			return m_pRBNode->m_key;
		}
		TValue& value()
		{
			return m_pRBNode->m_val;
		}
		
		bool isNull()
		{
			return m_pCurNode == NULL || m_pRBNode == NULL;
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
				TBTreeNode *pNode = m_pTree->getNode(m_pCurNode->next()); //править перенести установку флага занятостив  дерево
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
		TTreeNode* m_pRBNode;
	};
}

#endif