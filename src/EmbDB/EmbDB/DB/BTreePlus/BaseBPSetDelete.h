
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

	return TBTreeNodePtr(nullptr);
}


BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::remove(const TKey& key)
{
	TBTreeNodePtr pNode = findLeafNodeForRemove(key);
	if (!pNode.get())
		return false;

	assert(pNode->isLeaf());
	int32 nIndex = pNode->binary_search(m_comp, key);

	if (nIndex == -1)
		return false;

	RemoveFromLeafNode(pNode, nIndex, key);
	return true;
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::RemoveFromLeafNode(TBTreeNodePtr pNode, int32 nIndex, const TKey& key)
{
	
	pNode->removeByIndex(nIndex);
	pNode->setFlags(CHANGE_NODE, true);
	
	if (pNode->addr() == m_nRootAddr)
		return;

	int nFoundIndex = pNode->foundIndex();
	TBTreeNodePtr pParentNode = getParentNode(pNode.get());
	if (!pParentNode.get())
		return;

	if (!pNode->isHalfEmpty())
	{

		if (pNode->foundIndex() != -1 && pParentNode->isKey(m_comp, key, pNode->foundIndex()))
		{
			pParentNode->updateKey(pNode->foundIndex(), pNode->key(0));
			pParentNode->setFlags(CHANGE_NODE, true);
		}
		RemoveFromInnerNode(pParentNode, key);
		return;
	}



	assert(pParentNode.get());
	assert(!pParentNode->isLeaf());


	TBTreeNodePtr pDonorNode(nullptr);
	bool bLeft = false; //The position of the donor node relative to the node
	if (pParentNode->less() == pNode->addr())
	{
		pDonorNode = getNode(pParentNode->link(0));
		pDonorNode->setParent(pParentNode, 0);
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
			pLeafNodeLeft->setParent(pParentNode);
			if (pParentNode->count() > 1)
			{
				pLeafNodeRight = getNode(pParentNode->link(1));
				pLeafNodeRight->setParent(pParentNode, 1);
			}
		}
		else
		{
			pLeafNodeLeft = getNode(pParentNode->link(pNode->foundIndex() - 1));
			pLeafNodeLeft->setParent(pParentNode, pNode->foundIndex() - 1);

			if ((int32)pParentNode->count() > pNode->foundIndex() + 1)
			{
				pLeafNodeRight = getNode(pParentNode->link(pNode->foundIndex() + 1));
				pLeafNodeRight->setParent(pParentNode, pNode->foundIndex() + 1);
			}
		}

		assert(pLeafNodeLeft.get() != nullptr || pLeafNodeRight.get() != nullptr);
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
	if (!bUnion && !m_bMinSplit)
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
			UnionLeafNode(pParentNode.get(), pNode.get(), pDonorNode.get(), bLeft);
			nFoundIndex = pNode->foundIndex();
		}
	
	}
	else if (bAlignment)
	{
		AlignmentLeafNode(pParentNode.get(), pNode.get(), pDonorNode.get(), bLeft);		

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
	RemoveFromInnerNode(pParentNode, key);
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::UnionLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode*pDonorNode, bool bLeft)
{
	pLeafNode->UnionWith(pDonorNode, bLeft, nullptr);
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

	m_pTransaction->dropFilePage(pNode->m_nPageAddr, m_nNodesPageSize);
	pNode->setFlags(REMOVE_NODE, true);
	m_Cache.remove(pNode->m_nPageAddr);
 
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::RemoveFromInnerNode(TBTreeNodePtr pCheckNode, const TKey& key)
{ 
	while (pCheckNode.get())
	{
		TBTreeNodePtr  pParentNode = getParentNode(pCheckNode.get());
		if (!pParentNode.get())
		{
			if (!pCheckNode->count())
			{
				TBTreeNodePtr pNode = getNode(pCheckNode->less());
				assert(pNode.get());//Должна быть

				if (pNode->isLeaf())
				{
					pCheckNode->clear();
					pCheckNode->TransformToLeaf(m_pTransaction);
					pCheckNode->UnionWith(pNode.get(), false);
					pCheckNode->setFlags(ROOT_NODE, true);
				}
				else
				{
					pCheckNode->setLess(-1);
					pCheckNode->UnionInnerWith(pNode.get(), NULL, true);
				}
				m_Cache.remove(pNode->addr());
				deleteNode(pNode.get());
			}
			return;
		}


		if (pCheckNode->foundIndex() != -1 && pParentNode->isKey(m_comp, key, pCheckNode->foundIndex()))
		{
			TBTreeNodePtr pIndexNode = getNode(pParentNode->link(pCheckNode->foundIndex()));
			TBTreeNodePtr pMinNode = getMinimumNode(pIndexNode);
			pParentNode->updateKey(pCheckNode->foundIndex(), pMinNode->key(0));
			pParentNode->setFlags(CHANGE_NODE, true);
			//m_ChangeNode.insert(pParentNode);
		}
		if (!(pCheckNode->getFlags() & CHECK_REM_NODE))
		{
			pCheckNode = getParentNode(pCheckNode.get());//		break;
			continue;
		}
		pCheckNode->setFlags(CHECK_REM_NODE, false);
		if (!pCheckNode->isHalfEmpty())
		{
			pCheckNode = getParentNode(pCheckNode.get()); 
			continue;
		}

		TBTreeNodePtr pDonorNode(NULL);
		bool bLeft = false;


		if (pParentNode->less() == pCheckNode->addr())
		{
			pDonorNode = getNode(pParentNode->link(0));
			pDonorNode->setParent(pParentNode, 0);
			bLeft = false;
		}
		else
		{

			TBTreeNodePtr pLeafNodeRight(NULL);
			TBTreeNodePtr pLeafNodeLeft(NULL);

			if (pCheckNode->foundIndex() == 0)
			{
				pLeafNodeLeft = getNode(pParentNode->less());
				pLeafNodeLeft->setParent(pParentNode);
				if (pParentNode->count() > 1)
				{
					pLeafNodeRight = getNode(pParentNode->link(1));
					pLeafNodeRight->setParent(pParentNode, 1);
				}
			}
			else
			{
				pLeafNodeLeft = getNode(pParentNode->link(pCheckNode->foundIndex() - 1));
				pLeafNodeLeft->setParent(pParentNode, pCheckNode->foundIndex() - 1);
				if ((int32)pParentNode->count() > pCheckNode->foundIndex() + 1)
				{
					pLeafNodeRight = getNode(pParentNode->link(pCheckNode->foundIndex() + 1));
					pLeafNodeRight->setParent(pParentNode, pCheckNode->foundIndex() + 1);
				}
			}

			assert(pLeafNodeLeft.get() != NULL || pLeafNodeRight.get() != NULL);
			assert(pLeafNodeLeft != pCheckNode && (pLeafNodeRight != pCheckNode) && (pLeafNodeLeft != pLeafNodeRight));


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

		bool bUnion = false;
		bool bAlignment = false;

		bUnion = pCheckNode->IsHaveUnion(pDonorNode.get());
		if (!bUnion && !m_bMinSplit)
		{
			bAlignment = pCheckNode->IsHaveAlignment(pDonorNode.get());
		}
		if (bUnion)
		{
			if (pDonorNode->addr() == pParentNode->less())
				UnionInnerNode(pParentNode, pDonorNode, pCheckNode, false);
			else
				UnionInnerNode(pParentNode, pCheckNode, pDonorNode, bLeft);
		}
		else if (bAlignment)
		{
			AlignmentInnerNode(pParentNode, pCheckNode, pDonorNode, bLeft);

			assert(pCheckNode->count());
			assert(pDonorNode->count());
			assert(pParentNode->count());

		}
		pCheckNode = pParentNode;
	}
 
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::UnionInnerNode(TBTreeNodePtr pParentNode, TBTreeNodePtr pNode, TBTreeNodePtr pDonorNode, bool bLeft)
{

	TBTreeNodePtr pMinNode = bLeft ? getMinimumNode(getNode(pNode->less())) : getMinimumNode(getNode(pDonorNode->less()));
	pNode->UnionInnerWith(pDonorNode.get(), &pMinNode->key(0), bLeft);
	pNode->setFlags(CHANGE_NODE, true);

	pParentNode->removeByIndex(pDonorNode->foundIndex());
	if (bLeft && pNode->foundIndex() != -1)
	{
		pNode->setFoundIndex(pNode->foundIndex() - 1);
		assert(pNode->foundIndex() >= 0);
		pMinNode = getMinimumNode(getNode(pNode->less()));
		pParentNode->updateKey(pNode->foundIndex(), pMinNode->key(0));
	}
	deleteNode(pDonorNode.get());
	SetParentInChildCacheOnly(pNode);

	assert(pParentNode->count() != 0 || pParentNode->less() != -1);
	pParentNode->setFlags(CHANGE_NODE | CHECK_REM_NODE, true);
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::AlignmentInnerNode(TBTreeNodePtr pParentNode, TBTreeNodePtr pNode, TBTreeNodePtr pDonorNode, bool bLeft)
{

	TBTreeNodePtr pMinNode = bLeft ? getMinimumNode(getNode(pNode->less())) : getMinimumNode(getNode(pDonorNode->less()));
	if (!pNode->AlignmentInnerNodeOf(pDonorNode.get(), pMinNode->key(0), bLeft))
		return;

	if (!bLeft) // Node donor is on the right
	{
		pMinNode = getMinimumNode(getNode(pDonorNode->less()));
		pParentNode->updateKey(pDonorNode->foundIndex(), pMinNode->key(0));
		pParentNode->setFlags(CHANGE_NODE, true);
	}
	else
	{
		pMinNode = getMinimumNode(getNode(pNode->less()));
		pParentNode->updateKey(pNode->foundIndex(), pMinNode->key(0));
		pParentNode->setFlags(CHANGE_NODE, true);
	}

	SetParentInChildCacheOnly(pNode);
	SetParentInChildCacheOnly(pDonorNode);

	pParentNode->setFlags(CHANGE_NODE, true);
	pNode->setFlags(CHANGE_NODE, true);
	pDonorNode->setFlags(CHANGE_NODE, true);
}


BPSETBASE_TEMPLATE_PARAMS
std::shared_ptr<_TBTreeNode>  BPSETBASE_DECLARATION::getMinimumNode(TBTreeNodePtr pNode)
{

	TBTreeNodePtr pMinNode = pNode;
	while (!pMinNode->isLeaf())
	{

		pMinNode = getNode(pNode->less());
		pMinNode->setParent(pNode, -1);
		pNode = pMinNode;
		assert(pMinNode.get());
	}
	assert(pMinNode.get());
	return pMinNode;
}