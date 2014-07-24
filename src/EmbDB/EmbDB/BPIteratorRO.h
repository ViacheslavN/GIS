#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_ITERATOR_RO_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_ITERATOR_RO_H_
#include "BPNodeRO.h"

namespace embDB
{

	template <class _TKey, class _TValue, class _TLink, class _TComp, class _TBreeNode, class _Transaction, class _TInnerComp, class _TLeafComp>
	class TBaseBPlusTreeRO;


	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction, class _TInnerComp, class _TLeafComp,
	class _TBreeNode = BPTreeNodeRO<_TKey, _TValue, _TLink, _TComp> >
	class TBPTIteratorRO
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

		typedef typename TBTreeNode::TLeafObj TLeafObj; 
		typedef typename TBTreeNode::TLeafMemSet TLeafMemSet;
		typedef TBaseBPlusTreeRO<TKey, TValue, TLink, TComp, Transaction, TInnerCompressor, TLeafCompressor, TBTreeNode> TBTree;
		TBPTIteratorRO(TBTree *pTree, TBTreeNode *pCurNode, uint32 nCurIndex) :
			m_pTree(pTree), m_pCurNode(pCurNode), m_nCurIndex(nCurIndex)
		{
			if(m_pCurNode)
			{
				assert(m_pCurNode->isLeaf());
				if(m_pCurNode)
				{
					m_pCurNode->setFlags(BUSY_NODE, true);
				}
			}
		}
		TBPTIteratorRO(const TBPTIteratorRO& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nCurIndex = iter.m_nCurIndex;

		}
		~TBPTIteratorRO()
		{
			if(m_pCurNode)
				m_pCurNode->setFlags(BUSY_NODE, false);
		}
		TBPTIteratorRO& operator = (const TBPTIteratorRO& iter)
		{
			if(m_pCurNode)
				m_pCurNode->setFlag(BUSY_NODE, false);
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nCurIndex = iter.m_nCurIndex;
			return this;
		}

        const TKey& key() const
		{
			const TLeafMemSet& leafMemSet = m_pCurNode->m_leafMemset;
			return leafMemSet[m_nCurIndex].m_key;
		}
		const TValue& value()  const
		{
			const TLeafMemSet& leafMemSet = m_pCurNode->m_leafMemset;
			return leafMemSet[m_nCurIndex].m_val;
		}

		TKey& key()
		{
			TLeafMemSet& leafMemSet = m_pCurNode->m_leafMemset;
			return leafMemSet[m_nCurIndex].m_key;
		}
		TValue& value()
		{
			TLeafMemSet& leafMemSet = m_pCurNode->m_leafMemset;
			return leafMemSet[m_nCurIndex].m_val;
		}


		bool isNull()
		{
			return m_pCurNode == NULL;
		}

		bool next()
		{
			if(!m_pCurNode)
				return false;
			TLeafMemSet& leafMemSet = m_pCurNode->m_leafMemset;
			m_nCurIndex++;
			if(m_nCurIndex < leafMemSet.size())
				return true;
			
			{
				m_nCurIndex = 0;
				TBTreeNode *pNode = m_pTree->getNode(m_pCurNode->m_nNext, false, false, true); //править перенести установку флага занятостив  дерево
				m_pCurNode->setFlags(BUSY_NODE, false); 
				if(!pNode)
				{
					m_pCurNode = NULL;
					return false;

				}
				else
				{
					pNode->setFlags(BUSY_NODE, true); 
					m_pCurNode = pNode;
					TLeafMemSet& leafMemSet = m_pCurNode->m_leafMemset;
					if(leafMemSet.size())
					{
						return true;
					}
					else
					{
						pNode->setFlags(BUSY_NODE, false); 
						m_pCurNode = NULL;
						return false;
					}
				}
			}
			return false;
		}
	private:
		TBTree *m_pTree;
		TBTreeNode *m_pCurNode;
		uint32 m_nCurIndex;
	};
}

#endif