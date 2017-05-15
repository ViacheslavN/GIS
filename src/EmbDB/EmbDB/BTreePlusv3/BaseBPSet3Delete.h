
BPSETBASE_TEMPLATE_PARAMS
std::shared_ptr<_TBTreeNode> BPSETBASE_DECLARATION::findLeafNodeForRemove(const TKey& key)
{
	if (!m_pRoot.get())
	{
		if (!checkRoot())
			return TBTreeNodePtr(nullptr);
	}
	if (m_pRoot->isLeaf())
	{
		return m_pRoot;
	}
	else
	{
		int32 nIndex = -1;
		int64 nNextAddr = m_pRoot->findNodeRemove(m_comp, key, nIndex);
		TBTreeNodePtr pParent = m_pRoot;
		while (nNextAddr != -1)
		{
			TBTreeNodePtr pNode = getNode(nNextAddr);
			pNode->setParent(pParent, nIndex);
			if (pNode->isLeaf())
			{
				return pNode;
				break;
			}
			pParent = pNode;
			nNextAddr = pNode->findNodeRemove(m_comp, key, nIndex);
		}
	}

	return TBTreeNodePtr(NULL);
}


BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::remove(const TKey& key)
{
	TBTreeNodePtr pNode = findLeafNodeForRemove(key);
	if (!pNode.get())
		return false;

	assert(pNode->isLeaf());
	int32 nIndex = pNode->search(key);

	if (nIndex == -1)
		return false;

	return RemoveFromLeafNode(pNode, nIndex, key);
}


BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::RemoveFromLeafNode(TBTreeNodePtr& pNode, int32 nIndex, const TKey& key)
{


	pNode->removeByIndex(nIndex);
	pNode->setFlags(CHANGE_NODE, true);
	
	if (pNode->addr() = m_nRootAddr)
		return true;

	int nFoundIndex = pNode->foundIndex();
	TBTreeNodePtr pParentNode = getParentNode(pNode.get())
	if (!pParentNode.get())
		return true;

	if (!pLeafNode->isHalfEmpty())
	{

		if (pNode->foundIndex() != -1 && pParentNode->isKey(m_comp, key, pNode->foundIndex()))
		{
			pParentNode->updateKey(pLeafNode->foundIndex(), pLeafNode->key(0));
			pParentNode->setFlags(CHANGE_NODE, true);
		}
		return RemoveFromInnerNode( pParentNode.get(), key);
	}



	assert(pParentNode.get());
	assert(!pParentNode->isLeaf());


	TBTreeNodePtr pDonorNode(nullptr);
	bool bLeft = false; //The position of the donor node relative to the node
	if (pParentNode->less() == pNode->addr())
	{
		pDonorNode = getNode(pParentNode->link(0));
		pDonorNode->setParent(pParentNode.get(), 0);
		bLeft = false;
		assert(pDonorNode->isLeaf());
	}
	else
	{

		TBTreeNodePtr pLeafNodeRight(nullptr);
		TBTreeNodePtr pLeafNodeLeft(nullptr);

		if (pNode->foundIndex() == 0)
		{
			pLeafNodeLeft = getNode(pParentNode->less());
			pLeafNodeLeft->setParent(pParentNode.get());
			if (pParentNode->count() > 1)
			{
				pLeafNodeRight = getNode(pParentNode->link(1));
				pLeafNodeRight->setParent(pParentNode.get(), 1);
			}
		}
		else
		{
			pLeafNodeLeft = getNode(pParentNode->link(pNode->foundIndex() - 1));
			pLeafNodeLeft->setParent(pParentNode.get(), pNode->foundIndex() - 1);

			if ((int32)pParentNode->count() > pNode->foundIndex() + 1)
			{
				pLeafNodeRight = getNode(pParentNode->link(pNode->foundIndex() + 1));
				pLeafNodeRight->setParent(pParentNode.get(), pNode->foundIndex() + 1);
			}
		}

		assert(pLeafNodeLeft.get() != NULL || pLeafNodeRight.get() != NULL);
		assert(pLeafNodeLeft != pNode && (pLeafNodeRight != pNode) && (pLeafNodeLeft != pLeafNodeRight));

		uint32 nLeftCount = pLeafNodeLeft.get() ? pLeafNodeLeft->count() : 0xFFFFFFFF;
		uint32 nRightCount = pLeafNodeRight.get() ? pLeafNodeRight->count() : 0xFFFFFFFF;

		if (nLeftCount < nRightCount)
		{
			pDonorNode = pLeafNodeLeft;
			bLeft = true;
		}
		else
		{
			pDonorNode = pLeafNodeRight;
			bLeft = false;
		}
	}
	assert(pDonorNode.get());
	bool bUnion = pNode->IsHaveUnion(pDonorNode.get());
	bool bAlignment = false;
	if (!bUnion)
	{
		bAlignment = pNode->IsHaveAlignment(pDonorNode.get());
	} 
	if (bUnion)
	{

		if (pDonorNode->foundIndex() == -1)
		{
			UnionLeafNode(pParentNode.get(), pDonorNode.get(), pNode.get(), false);
			nFoundIndex = -1;
		}
		else
		{
			bRet = UnionLeafNode(pParentNode.get(), pNode.get(), pDonorNode.get(), bLeft);
			nFoundIndex = pNode->foundIndex();
		}
	
	}
	else if (bAlignment)
	{
		if (!AlignmentLeafNode(pParentNode.get(), pNode.get(), pDonorNode.get(), bLeft))
			return false;

		assert(pNode->count());
		assert(pDonorNode->count());
		nFoundIndex = pNode->foundIndex();
	}
	if (nFoundIndex != -1 && pParentNode->isKey(m_comp, key, nFoundIndex))
	{
		TBTreeNodePtr pIndexNode = getNode(pParentNode->link(nFoundIndex));
		pParentNode->updateKey(nFoundIndex, pIndexNode->key(0));
		pParentNode->setFlags(CHANGE_NODE, true);
	}

	return RemoveFromInnerNode(pParentNode, key);

}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::UnionLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode*pDonorNode, bool bLeft)
{
	pLeafNode->UnionWith(pDonorNode, bLeft, nCheckIndex);
	pLeafNode->setFlags(CHANGE_NODE, true);
	if (bLeft)
	{
		TBTreeNodePtr pPrevNode = getNode(pDonorNode->prev());
		if (pPrevNode.get())
		{
			assert(pPrevNode->isLeaf());
			pLeafNode->setPrev(pPrevNode->m_nPageAddr);
			pPrevNode->setNext(pLeafNode->m_nPageAddr);
			pPrevNode->setFlags(CHANGE_NODE, true);
		}
		else
			pLeafNode->setPrev(-1);
	}
	else
	{
		TBTreeNodePtr pNextNode = getNode(pDonorNode->m_LeafNode.m_nNext);
		if (pNextNode.get())
		{
			assert(pNextNode->isLeaf());
			pLeafNode->setNext(pNextNode->m_nPageAddr);
			pNextNode->setPrev(pLeafNode->m_nPageAddr);
			pNextNode->setFlags(CHANGE_NODE, true);
		}
		else
			pLeafNode->m_LeafNode.m_nNext = -1;
	}

	pParentNode->removeByIndex(pDonorNode->foundIndex());
	if (bLeft && pLeafNode->foundIndex() != -1)
	{
		pLeafNode->setFoundIndex(pLeafNode->foundIndex() - 1);
		assert(pLeafNode->foundIndex() >= 0);
		pParentNode->updateKey(pLeafNode->foundIndex(), pLeafNode->key(0));
	}
	deleteNode(pDonorNode);
	pParentNode->setFlags(CHANGE_NODE|CHECK_REM_NODE, true);
 
 }



BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::AlignmentLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode* pDonorNode, bool bLeft)
{
	assert(pDonorNode->count());
	if (!pLeafNode->AlignmentOf(pDonorNode, bLeft))
		return;
	if (bLeft)
	{
		pParentNode->updateKey(pLeafNode->foundIndex(), pLeafNode->key(0));
		pLeafNode->setFoundIndex(-1);
	}
	else
		pParentNode->updateKey(pDonorNode->foundIndex(), pDonorNode->key(0));

	pLeafNode->setFlags(CHANGE_NODE, true);
	pDonorNode->setFlags(CHANGE_NODE, true);
	pParentNode->setFlags(CHANGE_NODE, true);
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::deleteNode(TBTreeNode* pNode)
{

	if (m_pBPTreeStatistics)
		m_pBPTreeStatistics->DeleteNode(pNode->isLeaf());
	m_pTransaction->dropFilePage(pNode->m_nPageAddr, m_nNodesPageSize);
	pNode->setFlags(REMOVE_NODE, true);
	m_Cache.remove(pNode->m_nPageAddr);
 
}


BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::RemoveFromInnerNode(TBTreeNodePtr& pNode, const TKey& key)
{
	return true;
}