#pragma once


namespace embDB
{
	

	template <class _TKey, class _TComp, class _Transaction, class _TInnerCompess, class _TLeafCompess,
		class _TInnerNode, class _TLeafNode, class _TBTreeNode>
		class TBPSetIteratorV3
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
		typedef std::shared_ptr<TBTreeNode> TBTreeNodePtr;

		typedef STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TKeyMemSet;
 
 
		typedef TBPlusTreeSetV3<TKey, TComp, Transaction, TInnerCompess, TLeafCompess, TInnerNode, TBTreeLeafNode, TBTreeNode> TBTree;

		TBPSetIteratorV3(TBTree *pTree, TBTreeNodePtr& pCurNode, int32 nIndex) :
			m_pTree(pTree), m_pCurNode(pCurNode), m_nIndex(nIndex)
		{
			if (m_pCurNode.get())
			{
				assert(m_pCurNode->isLeaf());
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}


		}

		TBPSetIteratorV3() :  m_pTree(nullptr), m_nIndex(0), m_pCurLeafNode(nullptr)
		{

		}

		TBPSetIteratorV3(const TBPSetIteratorV3& iter)
		{
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
			if (m_pCurNode.get())
			{
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			}

		}
		~TBPSetIteratorV3()
		{

		}
		TBPSetIteratorV3& operator = (const TBPSetIteratorV3& iter)
		{
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;;
			m_pCurLeafNode = NULL;
			if (m_pCurNode.get())
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
		bool isNull() const
		{
			if (m_pCurNode.get() == NULL || m_nIndex == -1)
				return true;

			return false;

		}

		bool next()
		{
			if (isNull())
				return false;
 
			m_nIndex++;
			if ((uint32)m_nIndex < m_pCurLeafNode->count())
				return true;

			if (m_pCurNode->next() != -1)
			{
				TBTreeNodePtr pNode = m_pTree->getNode(m_pCurNode->next(), false, false, true);  
				if (!pNode.get())
				{
					m_pTree->getTransactions()->error(L"BTree Iter error load next node %I64d", m_pCurNode->next());
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
			if (isNull())
				return false;
			//m_pTree->ClearChache();
			m_nIndex--;
			if (m_nIndex >= 0)
				return true;

			if (m_pCurNode->prev() != -1)
			{
				TBTreeNodePtr pNode = m_pTree->getNode(m_pCurNode->prev(), false, true); //TO DO set busy
				if (!pNode.get())
				{
					m_pTree->getTransactions()->error(L"BTree Iter error load prev node %I64d", m_pCurNode->next());
					return false;
				}

				m_pCurNode = pNode;
				m_pCurLeafNode = &m_pCurNode->m_LeafNode;
				m_nIndex = m_pCurLeafNode->count() - 1;
				return true;
			}
			return false;
		}

		int64 addr() const
		{
			if (isNull())
				return -1;
			return m_pCurNode->addr();
		}
		int32 pos() const
		{
			return m_nIndex;
		}

		bool setAddr(int64 nAddr, int32  nPos)
		{

			if (nAddr == -1)
			{
				m_pCurLeafNode = NULL;
				m_pCurNode = NULL;
				return true;
			}

			TBTreeNodePtr pNode = m_pTree->getNode(nAddr, false, true);
			if (!pNode.get())
			{
				m_pTree->getTransactions()->error(L"BTree Iter error set addr node %I64d", nAddr);
				return false;
			}
			m_pCurNode = pNode;
			m_pCurLeafNode = &m_pCurNode->m_LeafNode;
			m_nIndex = nPos;
			return true;
		}
	public:
		TBTree *m_pTree;
		TBTreeNodePtr m_pCurNode;
		TBTreeLeafNode*	m_pCurLeafNode;
		int32 m_nIndex;
	};
}
