#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_ITERATOR_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_ITERATOR_MAP_H_
#include "BPMapTraits.h"
#include "BPTreeNodeMap.h"

namespace embDB
{

	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
	class _TInnerMemSet,	class _TLeafMemSet,	class _TInnerCompess,	class _TLeafCompess,
	class _TInnerNode,	class _TLeafNode ,		class _TBTreeNode>	class TBPlusTreeMap;


	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
	class _TInnerMemSet,	class _TLeafMemSet,	class _TInnerCompess,	class _TLeafCompess,
	class _TInnerNode,	class _TLeafNode ,	class _TBTreeNode>
	class TBPTIteratorMap
	{
	public:

	
		typedef _TKey      TKey;
		typedef _TValue    TValue;
		typedef _TComp	   TComp;
		typedef _TLink     TLink;
		typedef	_Transaction  Transaction;
		typedef _TBTreeNode  TBTreeNode;
		typedef _TLeafMemSet TLeafMemSet;
		typedef typename TLeafMemSet::TTreeNode TLeftMemSetNode;

		typedef TBPlusTreeMap<_TKey,  _TValue,  _TLink,  _TComp,  _Transaction,
		 _TInnerMemSet,	 _TLeafMemSet,	_TInnerCompess,	 _TLeafCompess,	 _TInnerNode,	 _TLeafNode , _TBTreeNode> TBTree;

		TBPTIteratorMap(TBTree *pTree, TBTreeNode *pCurNode, TLeftMemSetNode* pRBNode) :
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

		TBPTIteratorMap(const TBPTIteratorMap& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_pRBNode = iter.m_pRBNode;

		}
		~TBPTIteratorMap()
		{
			if(m_pCurNode)
				m_pCurNode->setFlags(BUSY_NODE, false);
		}
		TBPTIteratorMap& operator = (const TBPTIteratorMap& iter)
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
			if(m_pCurNode == NULL || m_pRBNode == NULL)
				return true;
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;
			return leafMemSet.isNull(m_pRBNode);
		}

		bool next()
		{
			if(!m_pCurNode)
				return false;
			//m_pTree->ClearChache();
			TLeafMemSet& leafMemSet = m_pCurNode->m_LeafNode.m_leafMemSet;
			if(leafMemSet.isNull(m_pRBNode->m_pNext))
			{
				m_pRBNode = NULL;
				TBTreeNode *pNode = m_pTree->getNode(m_pCurNode->next(), false, false, true); //править перенести установку флага занятостив  дерево
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