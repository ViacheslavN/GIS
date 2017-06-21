#pragma once
#include "CommonLibrary/general.h"

namespace embDB
{

	class IReadTreeCancel
	{
		public:
			virtual bool IsCancel() = 0;
	};
	template <class _TKey, class _TBTreeNode, class _TBTree>
	class TBPSetInfoTree
	{
	public:
		typedef _TKey      TKey;
		typedef int64     TLink;
		typedef _TBTreeNode TBTreeNode;
		typedef std::shared_ptr<TBTreeNode> TBTreeNodePtr;

		typedef STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TKeyMemSet;
		typedef _TBTree TBTree;

		struct SNodeInfo
		{

			SNodeInfo(TBTreeNodePtr _pBTNode, int32 _nPos) : pBTNode(_pBTNode), nPos(_nPos)
			{}

			TBTreeNodePtr pBTNode;
			int32 nPos;
		};
		TBPSetInfoTree(TBTree *pTree) :
			m_pTree(pTree), m_bFinishRead(false)
		{
			clear();
		}
 	 
		~TBPSetInfoTree()
		{

		}

		bool CalcNodesInTree(IReadTreeCancel *pCancel = nullptr)
		{
			m_bFinishRead = false;
			clear();

			TBTreeNodePtr pRootNode = m_pTree->root();

			if (!pRootNode.get())
				return false;

			

			if (pRootNode->isLeaf())
			{
				m_nLeafNodeCount += 1;
				m_bFinishRead = true;
				return true;
			}

			std::stack<SNodeInfo> nNodes;

			nNodes.push(SNodeInfo(pRootNode, -1));

			TBTreeNodePtr pNode;
			while (!nNodes.empty())
			{
				auto& node = nNodes.top();
				if (node.nPos == node.pBTNode->count())
				{
					nNodes.pop();
					continue;
				}

				int32 nPos = node.nPos++;

				
				
				pNode = m_pTree->getNode(nPos == -1 ? node.pBTNode->less() : node.pBTNode->link(nPos));
				if (!pNode->isLeaf())
				{
					if (nPos == -1)
					{
						m_nInnerNodeCount += (node.pBTNode->count() + 1);
					}

					nNodes.push(SNodeInfo(pNode, -1));
					continue;
				}

				node.nPos = node.pBTNode->count();
				m_setHeights.insert(nNodes.size());
				CountLeafNode(node.pBTNode, pNode);

			}




			m_bFinishRead = true;
			return true;
		}


		void clear()
		{
			m_nLeafNodeCount = 0;
			m_nInnerNodeCount = 0;
			m_nKeyCount = 0;
			m_nHeight = 0;
			m_setHeights.clear();
		}
	 
		void CountLeafNode(TBTreeNodePtr& pParentNode, TBTreeNodePtr& pNode)
		{
			m_nLeafNodeCount += (pParentNode->count() + 1);
			m_nKeyCount += (pParentNode->count() + 1) *pNode->count();
		}


		 

	public:

		

		TBTree *m_pTree;
		bool m_bFinishRead;

		uint64 m_nLeafNodeCount;
		uint64 m_nInnerNodeCount;
		uint64 m_nKeyCount;
		uint64 m_nHeight;
		std::set<uint64> m_setHeights;
	};
}
