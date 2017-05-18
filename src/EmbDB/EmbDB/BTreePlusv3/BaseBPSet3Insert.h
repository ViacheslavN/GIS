#pragma once
 

BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::insert(const TKey& key)
{
	TBTreeNodePtr pNode = findLeafNodeForInsert(key);
	if (!pNode.get())
		return false;

	if (pNode->insertInLeaf(m_comp, key) == -1)
		return false;

	pNode->setFlags(CHANGE_NODE, true);

	CheckLeafNode(pNode.get(), false);
 
	return true;
}

BPSETBASE_TEMPLATE_PARAMS
std::shared_ptr<_TBTreeNode> BPSETBASE_DECLARATION::findLeafNodeForInsert(const TKey& key)
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
		int64 nNextAddr = m_pRoot->findNodeInsert(m_comp, key, nIndex);
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
			nNextAddr = pNode->findNodeInsert(m_comp, key, nIndex);
		}
	}

	return TBTreeNodePtr(NULL);
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::CheckLeafNode(TBTreeNode *pNode, bool bPreSave)
{


	if (bPreSave)
		pNode->PreSave(m_pTransaction);

	if (!pNode->isNeedSplit())
		return;

	if (pNode->addr() == m_nRootAddr)
	{
		TransformRootToInner();
		return;
	}
	TBTreeNodePtr  pCheckNode = getParentNode(pNode);
	TBTreeNodePtr pNewRightLeafNode = newNode(false, true);
	splitLeafNode(pNode, pNewRightLeafNode.get(), pCheckNode);

	while (pCheckNode->isNeedSplit())
	{
		TBTreeNodePtr pParentNode = getParentNode(pCheckNode.get());	
		if (!pParentNode.get())
		{
			assert(pCheckNode->addr() == m_nRootAddr);
			SplitRootInnerNode();
			break;
		}

		splitInnerNode(pCheckNode.get(), pParentNode);
		pCheckNode = pParentNode;

	}

}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::TransformRootToInner()
{
	TBTreeNodePtr pLeftNode = newNode(false, true);
	TBTreeNodePtr pRightNode = newNode(false, true);

	TKey splitKey;
	int nSplitIndex = m_pRoot->splitIn(pLeftNode.get(), pRightNode.get(), &splitKey);

	m_pRoot->clear();
	m_pRoot->TransformToInner(m_pTransaction);
	
	m_pRoot->setLess(pLeftNode->addr());
	int nInsertIndex = m_pRoot->insertInInnerNode(m_comp, splitKey, pRightNode->addr());

	pRightNode->setParent(m_pRoot, nInsertIndex);
	pLeftNode->setParent(m_pRoot, -1);
	pLeftNode->setNext(pRightNode->addr());
	pRightNode->setPrev(pLeftNode->addr());

	pLeftNode->setFlags(CHANGE_NODE, true);
	pRightNode->setFlags(CHANGE_NODE, true);
	m_pRoot->setFlags(CHANGE_NODE, true);

}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SplitRootInnerNode()
{

	TBTreeNodePtr pLeftNode = newNode(false, false);
	TBTreeNodePtr pRightNode = newNode(false, false);
	TKey nMedianKey;

	m_pRoot->splitIn(pLeftNode.get(), pRightNode.get(), &nMedianKey);
	int nIndex = m_pRoot->insertInInnerNode(m_comp, nMedianKey, pRightNode->addr());
	m_pRoot->setLess(pLeftNode->addr());

	pLeftNode->setParent(m_pRoot, -1);
	pRightNode->setParent(m_pRoot, nIndex);

	SetParentInChildCacheOnly(pLeftNode);
	SetParentInChildCacheOnly(pRightNode);


	pLeftNode->setFlags(CHANGE_NODE, true);
	pRightNode->setFlags(CHANGE_NODE, true);
	m_pRoot->setFlags(CHANGE_NODE, true);
 
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SetParentInChildCacheOnly(TBTreeNodePtr& pNode)
{
	assert(!pNode->isLeaf());
	TBTreeNodePtr pLessNode = m_Cache.GetElem(pNode->less(), true);
	if (pLessNode)
		pLessNode->setParent(pNode, -1);
	for (uint32 i = 0, sz = pNode->count(); i < sz; ++i)
	{
		TBTreeNodePtr pChildNode = m_Cache.GetElem(pNode->link(i), true);
		if (pChildNode.get())
			pChildNode->setParent(pNode, i);
	}
}

BPSETBASE_TEMPLATE_PARAMS
int BPSETBASE_DECLARATION::splitLeafNode(TBTreeNode *pNode, TBTreeNode *pNewNode, TBTreeNodePtr& pParentNode)
{
	assert(pNewNode->isLeaf());
	assert(pNode->isLeaf());

	TKey splitKey;
	int nSplitIndex = pNode->splitIn(pNewNode, &splitKey);
	int nInsertIndex = pParentNode->insertInInnerNode(m_comp, splitKey, pNewNode->m_nPageAddr);
	pNewNode->setParent(pParentNode, nInsertIndex);

	if (pNode->next() != -1)
	{
		pNewNode->m_LeafNode.m_nNext = pNode->m_LeafNode.m_nNext;
		TBTreeNodePtr pNextNode = getNode(pNode->m_LeafNode.m_nNext);
		if (pNextNode.get())
		{
			pNextNode->m_LeafNode.m_nPrev = pNewNode->m_nPageAddr;
			SetParentNext(pNode, pNextNode.get());
		}
		pNextNode->setFlags(CHANGE_NODE, true);
	}
	pNode->m_LeafNode.m_nNext = pNewNode->m_nPageAddr;
	pNewNode->m_LeafNode.m_nPrev = pNode->m_nPageAddr;

	pNode->setFlags(CHANGE_NODE, true);
	pNewNode->setFlags(CHANGE_NODE, true);
	pParentNode->setFlags(CHANGE_NODE, true);
	return nSplitIndex;
}



BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SetParentNext(TBTreeNode *pNode, TBTreeNode* pNodeNext)
{
	int nFoundIndex = pNode->foundIndex();
	TBTreeNodePtr pParent = getParentNode(pNode);
	if ((nFoundIndex + 1) < (int)pParent->count() || nFoundIndex == -1)
	{
		pNodeNext->setParent(pParent, nFoundIndex + 1);
		return;
	}


	nFoundIndex = pParent->foundIndex();
	pParent = getParentNode(pParent.get());

	while (pParent.get())
	{

		if ((nFoundIndex + 1) < (int)pParent->count() || nFoundIndex == -1)
		{
			break;
		}
		nFoundIndex = pParent->foundIndex();
		pParent = getParentNode(pParent.get());

	}
	if (!pParent.get())
		pParent = m_pRoot;

	TBTreeNodePtr pNextParentNode = getNode(pParent->link(nFoundIndex + 1));
	pNextParentNode->setParent(pParent, nFoundIndex + 1);

	TBTreeNodePtr pLessNode = getNode(pNextParentNode->less());

	while (!pLessNode->isLeaf())
	{
		pLessNode->setParent(pNextParentNode, -1);
		pNextParentNode = pLessNode;
		pLessNode = getNode(pNextParentNode->less());
	}

	assert(pNodeNext->addr() == pNextParentNode->less());
	pNodeNext->setParent(pNextParentNode, -1);
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::splitInnerNode(TBTreeNode *pNode, TBTreeNodePtr& pParentNode)
{
	if (!pParentNode.get())
	{
		SplitRootInnerNode();
		return;
	}

	TBTreeNodePtr pNodeNewRight = newNode(false, false);
	

	TKey nMedianKey;
	if (!pNode->splitIn(pNodeNewRight.get(), &nMedianKey))
		return;

	SetParentInChildCacheOnly(pNodeNewRight);

	int nIndex = pParentNode->insertInInnerNode(m_comp, nMedianKey, pNodeNewRight->m_nPageAddr);
	pNodeNewRight->setParent(pParentNode, nIndex);
	pParentNode->setFlags(CHANGE_NODE, true);
	pNodeNewRight->setFlags(CHANGE_NODE, true);
	pNode->setFlags(CHANGE_NODE, true);
}